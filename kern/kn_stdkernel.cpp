#include "kn_stdkernel.h"
#include "kn_timer.h"

#include <cassert>
#include <cstdlib>

using namespace KERN;

static StdKernel* s_singleton = nullptr; // this should be thread_local

//----------------------------------------------------------------------//
StdKernel::StdKernel()
{
  assert(s_singleton==nullptr);
  s_singleton = this;
}

//----------------------------------------------------------------------//
void StdKernel::registerTimer(CallbackTimer& new_timer)
{
  assert(s_singleton!=nullptr);
  
  //check if it already exists (debug)
#ifdef NDEBUG
  for (const auto& timer : s_singleton->d_timers)
    {
      assert(!timer.is(new_timer));
    }
#endif

  s_singleton->d_timers.push_back(&new_timer);
}

//----------------------------------------------------------------------//
void StdKernel::removeTimer(CallbackTimer& t)
{
  assert(s_singleton!=nullptr);
  assert(!s_singleton->d_timers.empty());
  
  //check if it already exists (debug)
#ifdef NDEBUG
  for (const auto& timer : s_singleton->d_timers)
    {
      assert(timer != new_timer);
    }
#endif

  s_singleton->d_timers.remove(&t);
}

//----------------------------------------------------------------------//
void StdKernel::process()
{
  assert(!d_timers.empty());
  
  for (const auto& timer : d_timers)
    {
      assert(timer != nullptr);
      if (!timer->isDisabled()) // if enabled
	{
	  timer->processTimeOut(); //update times and process timeouts
	}
    }
}

//----------------------------------------------------------------------//
void StdKernel::run()
{
  while (1)
    {
      process();
    }
}
