#include "kn_asiocallbacktimer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace KERN;

//----------------------------------------------------------------------//
AsioCallbackTimer::AsioCallbackTimer()
  : d_timer(new boost::asio::deadline_timer(AsioKernel::getService()))
{}

//----------------------------------------------------------------------//
AsioCallbackTimer::AsioCallbackTimer(std::string name)
  : d_name(std::move(name)),
    d_timer(new boost::asio::deadline_timer(AsioKernel::getService()))
{}

//----------------------------------------------------------------------//
void AsioCallbackTimer::setTimeoutCallback(std::function<void()> callback)
{
  d_timeout_callback = callback;
}
  
//----------------------------------------------------------------------//
void AsioCallbackTimer::restart(const std::chrono::milliseconds& time)
{
  setTime(time);
  restart();
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::singleShot(const std::chrono::milliseconds& time)
{
  restart(time);
  d_is_single_shot = true;
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::singleShotZero()
{
  singleShot(std::chrono::milliseconds(0));
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::singleShotZero(std::function<void()> callback)
{
  d_timeout_callback = callback;
  singleShotZero();
}
		  
//----------------------------------------------------------------------//
void AsioCallbackTimer::restartIfNotSet(const std::chrono::milliseconds& time)
{
  if (d_is_set)
    {
      return;
    }
  restart(time);
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::
restartIfNotSetOrDisabled(const std::chrono::milliseconds& time)
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
bool AsioCallbackTimer::isDisabled()
{
  return !d_is_enabled;
}

//----------------------------------------------------------------------//
long AsioCallbackTimer::getConseqTimeOuts()
{
  return d_count_conseq_timeouts;
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::timerCallBack(const boost::system::error_code& err,
				      boost::asio::deadline_timer* t)
{
  if (err == boost::asio::error::operation_aborted)
    {
      // Possibilities:
      // 1) Timer has been cancelled because async_wait() was not called before running the io_service.
      // 2) Timer has been cancelled because cancel() was called on the timer.
      // 3) Expiry time was reset before the wait time expired.
      std::cout << "AsioCallbackTimer::timerCallBack - timer aborted. timeout [ms] = " << d_timeout.count() << ". Name is: " << d_name << std::endl;
      return;
    }

  if (!d_is_enabled) 
    {
      // to catch timers that have already expired and have been queued, but need to be cancelled
      return;
    }
  
  assert(t != nullptr);
  ++d_count_conseq_timeouts;
  d_timeout_callback();
  
  if (!d_is_enabled || d_is_single_shot) 
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
      d_timer->expires_from_now(boost::posix_time::milliseconds(d_timeout.count()));
    }
  catch (...)
    {
      // LOG
      assert(false);
      return;
    }

  d_timer->async_wait([&](const boost::system::error_code& e){
      timerCallBack(e,d_timer.get());
    });
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::setTime(const std::chrono::milliseconds& time)
{
  d_timeout = time;
  d_is_set = true;
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::restart()
{
  d_count_conseq_timeouts = 0;
  d_is_enabled = true;

  scheduleCallback();
}
