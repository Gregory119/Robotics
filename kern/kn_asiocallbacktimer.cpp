#include "kn_asiocallbacktimer.h"

#include "kn_asiokernel.h"

#include <cassert>

using namespace KERN;

//----------------------------------------------------------------------//
AsioCallbackTimer::AsioCallbackTimer()
  : d_timer(new boost::asio::deadline_timer(AsioKernel::getService()))
{}

//----------------------------------------------------------------------//
void AsioCallbackTimer::setCallback(std::function<void()> callback)
{
  d_user_callback = callback;
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::restart()
{
  if (!d_user_callback)
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
  if (!d_user_callback)
    {
      // LOG
      assert(false);
      return;
    }

  d_count_conseq_timeouts = 0;
  d_is_enabled = true;
  setTimeMs(time_ms);

  // will cancel a pending timeout unless it has already timed out (callback is being called or is queued to be called)
  d_timer->expires_from_now(boost::posix_time::milliseconds(time_ms));
  d_timer->async_wait([&](const boost::system::error_code& e){
      timerCallBack(e,d_timer.get());
    });
}

//----------------------------------------------------------------------//
void AsioCallbackTimer::timerCallBack(const boost::system::error_code& err,
				      boost::asio::deadline_timer* t)
{
  if (err == boost::asio::error::operation_aborted)
    {
      // cancelled to be restarted
      return;
    }
  
  assert(t != nullptr);
  ++d_count_conseq_timeouts;
  d_user_callback(); // could be disabled here
  
  if (d_is_enabled) // schedule to call again
    {
      assert(isSet());
      d_timer->expires_from_now(boost::posix_time::milliseconds(d_timeout_ms));

      d_timer->async_wait([&](const boost::system::error_code& e){
	  timerCallBack(e,d_timer.get());
	});
    }
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
void AsioCallbackTimer::restartMsIfNotSetElseDisabled(long time_ms)
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
  d_is_enabled = false;
  d_count_conseq_timeouts = 0;
}
   
//----------------------------------------------------------------------//
void AsioCallbackTimer::setTimeMs(long time)
{
  assert(time>=0);
  d_timeout_ms = time;
}
