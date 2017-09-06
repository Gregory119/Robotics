#include "kn_callbacktimer.h"

#include "kn_stdkernel.h"

#include <cassert>

using namespace KERN;

//----------------------------------------------------------------------//
CallbackTimer::CallbackTimer()
{
  StdKernel::registerTimer(*this);
}

//----------------------------------------------------------------------//
CallbackTimer::setCallback(std::function<void()> callback)
{
  d_callback = callback;
}

//----------------------------------------------------------------------//
CallbackTimer::~CallbackTimer()
{
  StdKernel::removeTimer(*this);
}

//----------------------------------------------------------------------//
void CallbackTimer::processTimeOut()
{
  if (!d_is_enabled)
    {
      assert(false)
      return;
    }
  
  //check time passed against set time
  if (hasTimedOut())
    {
      resetAndStart(); //timeout timing
      ++d_count_conseq_timeouts;
      ++d_total_timeouts;

      if (!d_callback)
	{
	  assert(false);
	  return;
	}
      
      d_callback();
    }
}

//----------------------------------------------------------------------//
void CallbackTimer::restart()
{
  assert(isSet());
  resetAndStart();
  d_is_enabled = true;
  d_count_conseq_timeouts = 0;
}
  
//----------------------------------------------------------------------//
void CallbackTimer::restartMs(long time_ms)
{
  resetAndStart();
  setTimeMs(time_ms);
  d_is_enabled = true;
  d_count_conseq_timeouts = 0;
}

//----------------------------------------------------------------------//
void CallbackTimer::restartMsIfNotSet(long time_ms)
{
  if (!isSet())
    {
      restartMs(time_ms);
    }
}

//----------------------------------------------------------------------//
void CallbackTimer::disable()
{
  d_is_enabled = false;
  d_count_conseq_timeouts = 0;
}
   
//----------------------------------------------------------------------//
bool CallbackTimer::hasTimedOut()
{
  return d_timeout_ms <= getTimeMs();
}

//----------------------------------------------------------------------//
void CallbackTimer::setTimeMs(long time)
{
  assert(time>=0);
  d_timeout_ms = time;
}
