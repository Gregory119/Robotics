#include "kn_timer.h"

#include "kn_stdkernel.h"

#include <cassert>

using namespace KERN;

//----------------------------------------------------------------------//
KernelTimer::KernelTimer(KernelTimerOwner* o)
  : d_owner(o)
{
  StdKernel::registerTimer(*this);
}

//----------------------------------------------------------------------//
KernelTimer::KernelTimer(const KernelTimer& rhs)
  : KernelTimer(rhs.d_owner)
{}

//----------------------------------------------------------------------//
KernelTimer::~KernelTimer()
{
  StdKernel::removeTimer(*this);
}

//----------------------------------------------------------------------//
bool KernelTimer::processTimeOut()
{
  if (!d_is_enabled)
    {
      return true;
    }
  
  //check time passed against set time
  if (hasTimedOut())
    {
      resetAndStart(); //timeout timing
      ++d_count_conseq_timeouts;
      ++d_total_timeouts;
      
      if (!d_owner->handleTimeOut(*this))
	{
	  assert(false);
	  return false;
	}
    }

  return true;
}

//----------------------------------------------------------------------//
void KernelTimer::restart()
{
  assert(isSet());
  resetAndStart();
  d_is_enabled = true;
  d_count_conseq_timeouts = 0;
}
  
//----------------------------------------------------------------------//
void KernelTimer::restartMs(long time_ms)
{
  resetAndStart();
  setTimeMs(time_ms);
  d_is_enabled = true;
  d_count_conseq_timeouts = 0;
}

//----------------------------------------------------------------------//
void KernelTimer::restartMsIfNotSet(long time_ms)
{
  if (!isSet())
    {
      restartMs(time_ms);
    }
}

//----------------------------------------------------------------------//
void KernelTimer::disable()
{
  d_is_enabled = false;
  d_count_conseq_timeouts = 0;
}
   
//----------------------------------------------------------------------//
bool KernelTimer::hasTimedOut()
{
  return d_timeout_ms <= getTimeMs();
}

//----------------------------------------------------------------------//
void KernelTimer::setTimeMs(long time)
{
  assert(time>=0);
  d_timeout_ms = time;
}
