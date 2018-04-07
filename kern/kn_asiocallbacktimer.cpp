#include "kn_asiocallbacktimer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace KERN;

//----------------------------------------------------------------------//
AsioCallbackTimer::AsioCallbackTimer()
  : d_timer(new boost::asio::deadline_timer(AsioKernel::getService()))
{
  d_internal_callback = [this](const boost::system::error_code& err){
    timerCallback(err);
  };
}

//----------------------------------------------------------------------//
AsioCallbackTimer::AsioCallbackTimer(std::string name)
  : d_name(std::move(name)),
    d_timer(new boost::asio::deadline_timer(AsioKernel::getService()))
{
  d_internal_callback = [this](const boost::system::error_code& err){
    timerCallback(err);
  };
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::setTimeoutCallback(std::function<void()> callback)
{
  d_timeout_callback = callback;
}
  
//----------------------------------------------------------------------//
void AsioCallbackTimer::restart(const std::chrono::microseconds& time)
{
  setTime(time);
  restart();
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::singleShot(const std::chrono::microseconds& time)
{
  restart(time);
  d_is_single_shot = true;
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::singleShotZero()
{
  singleShot(std::chrono::microseconds(0));
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::singleShotZero(std::function<void()> callback)
{
  d_timeout_callback = callback;
  singleShotZero();
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::singleShot()
{
  restart();
  d_is_single_shot = true;
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::restartIfNotSet(const std::chrono::microseconds& time)
{
  if (d_is_set)
    {
      return;
    }
  restart(time);
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::
restartIfNotSetOrDisabled(const std::chrono::microseconds& time)
{
  if (!d_is_set || !d_is_enabled)
    {
      restart(time);
    }
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::disable()
{
  if (!d_is_enabled)
    {
      // log
      std::cerr << "The timer '" << d_name << "' has already been disabled." << std::endl;
      assert(false);
      return;
    }
  
  d_count_conseq_timeouts = 0;
  d_is_enabled = false;
  
  try
    {
      d_timer->cancel(); // forces the completion of any asynchronous wait operations
    }
  catch (...)
    {
      // not sure how to deal with this, but at least disable the timer
      // LOG
      assert(false);
    }
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::disableIfEnabled()
{
  if (d_is_enabled)
    {
      disable();
    }
}

//----------------------------------------------------------------------//
long AsioCallbackTimer::getConseqTimeOuts()
{
  return d_count_conseq_timeouts;
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::
setInternalTimerCallback(std::function<void(const boost::system::error_code& err)> f)
{
  d_internal_callback = f;
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::timerCallback(const boost::system::error_code& err)
{
  if (err == boost::asio::error::operation_aborted)
    {
      // Possibilities:
      // 1) Timer has been cancelled because async_wait() was not called before running the io_service.
      // 2) Timer has been cancelled because cancel() was called on the timer.
      // 3) Expiry time was reset before the wait time expired.
      //std::cout << "AsioCallbackTimer::timerCallback - timer '" << d_name << "' aborted. timeout [ms] = " << d_timeout.count() << ". Name is: " << d_name << std::endl;
      // already disabled => not scheduled
      return;
    }

  if (!d_is_enabled) 
    {
      // to catch timers that have already expired and have been queued, but need to be cancelled
      return;
    }

  if (d_is_single_shot)
    {
      d_is_enabled = false;
    }
  
  ++d_count_conseq_timeouts;
  d_timeout_callback(); // timer can be enabled here
  
  if (!d_is_enabled) 
    {
      // to catch disables requested in the user callback function
      return;
    }

  scheduleCallback();
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::scheduleCallback()
{
  if (!d_is_set)
    {
      assert(false);
      return;
    }
  
  if (!d_timeout_callback)
    {
      assert(false);
      return;
    }
  
  try
    {
      d_timer->expires_from_now(boost::posix_time::microseconds(d_timeout.count()));
    }
  catch (...)
    {
      // LOG
      assert(false);
      return;
    }

  d_is_enabled = true;
  
  d_timer->async_wait([&](const boost::system::error_code& e){
      d_internal_callback(e);
    });
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::setTime(const std::chrono::microseconds& time)
{
  d_timeout = time;
  d_is_set = true;
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::restart()
{
  d_count_conseq_timeouts = 0;
  disableIfEnabled();

  scheduleCallback(); // re-enables after cancel/disable
}
