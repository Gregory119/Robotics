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
  // each callback timer holds it own deadline timer
  // - starting the timer will recreate the deadline timer with the set time, followed by setting the callback function which is then used to set the boost timer function handler (with a check for an error)
  // the kernel holds an io service singleton which is used to create the deadline timer
  // - the kernel will call the run function on the io service
  // - the kernel will create a long duration continuous timer to always keep the io service run function busy
  
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
