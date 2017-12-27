#include "kn_asiocallbacktimer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace KERN;

//----------------------------------------------------------------------//
AsioCallbackTimer::AsioCallbackTimer()
  : AsioCallbackTimer(std::string())
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
void AsioCallbackTimer::restart()
{
  if (!d_timeout_callback)
    {
      // LOG
      assert(false);
      return;
    }
  
  assert(isSet());
  restartMs(d_timeout_ms);
}
  
//----------------------------------------------------------------------//
void AsioCallbackTimer::restartMs(long time_ms)
{
  if (!d_timeout_callback)
    {
      // LOG
      assert(false);
      return;
    }

  d_count_conseq_timeouts = 0;
  d_is_enabled = true;
  setTimeMs(time_ms);

  // will cancel a pending timeout unless it has already timed out (callback is being called or is queued to be called)
  try
    {
      d_timer->expires_from_now(boost::posix_time::milliseconds(time_ms));
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
void AsioCallbackTimer::restart(std::chrono::milliseconds time)
{
  restartMs(time.count());
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::singleShot(std::chrono::milliseconds time)
{
  restart(time);
  d_is_single_shot = true;
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
      std::cout << "AsioCallbackTimer::timerCallBack - timer aborted. timeout = " << d_timeout_ms << ". Name is: " << d_name << std::endl;
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

  // schedule to call again
  assert(isSet());
  try
    {
      d_timer->expires_from_now(boost::posix_time::milliseconds(d_timeout_ms));
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
void AsioCallbackTimer::restartMsIfNotSet(long time_ms)
{
  if (!isSet())
    {
      restartMs(time_ms);
    }
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::restartMsIfNotSetOrDisabled(long time_ms)
{
  if (!isSet()) // implies disabled
    {
      restartMs(time_ms);
    }
  else if (isDisabled()) // is set, but disabled
    {
      restart();
    }
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::disable()
{
  if (!d_is_enabled)
    {
      // log
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
void AsioCallbackTimer::setTimeMs(long time)
{
  assert(time>=0);
  d_timeout_ms = time;
}
