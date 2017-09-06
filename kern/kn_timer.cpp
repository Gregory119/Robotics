#include "kn_timer.h"

#include <cassert>

using namespace KERN;

//----------------------------------------------------------------------//
KernelTimer::KernelTimer(KernelTimerOwner* o)
  : d_owner(o),
    d_callback_timer(new CallbackTimer())
{
  d_callback_timer->setCallback([this](){
      if (!d_owner->handleTimeOut(*this))
	{
	  assert(false);
	  return;
	}});
}

//----------------------------------------------------------------------//
void KernelTimer::restart()
{
  d_callback_timer->restart();
}
  
//----------------------------------------------------------------------//
void KernelTimer::restartMs(long time_ms)
{
  d_callback_timer->restartMs(time_ms);
}

//----------------------------------------------------------------------//
void KernelTimer::restartMsIfNotSet(long time_ms)
{
  d_callback_timer->restartMsIfNotSet(time_ms);
}

//----------------------------------------------------------------------//
void KernelTimer::disable()
{
  d_callback_timer->disable();
}
   
//----------------------------------------------------------------------//
bool KernelTimer::hasTimedOut()
{
  return d_callback_timer->hasTimedOut();
}

//----------------------------------------------------------------------//
void KernelTimer::setTimeMs(long time)
{
  d_callback_timer->setTimeMs(time);
}
