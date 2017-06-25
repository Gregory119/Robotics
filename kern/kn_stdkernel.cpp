#include "kn_stdkernel.h"
#include "kn_timer.h"

#include <cassert>
#include <cstdlib>

using namespace KERN;

static StdKernel* s_singleton = nullptr;

//----------------------------------------------------------------------//
StdKernel::StdKernel()
{
  assert(s_singleton==nullptr);
  s_singleton = this;
}

//----------------------------------------------------------------------//
void StdKernel::registerTimer(KernelTimer& new_timer)
{
  assert(s_singleton!=nullptr);
  
  //check if it already exists (debug)
#ifdef NDEBUG
  for (const auto& timer : s_singleton->d_timers)
    {
      assert(timer != new_timer);
    }
#endif

  s_singleton->d_timers.push_back(&new_timer);
}

//----------------------------------------------------------------------//
int StdKernel::process()
{
  assert(!d_timers.empty());
  
  for (const auto& timer : d_timers)
    {
      assert(timer != nullptr);
      
      if (!timer->processTimeOut()) //update times and process timeouts
	{
	  assert(false);
	  return EXIT_FAILURE;
	}
    }
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------//
int StdKernel::run()
{
  int ret = EXIT_SUCCESS;
  while (ret == EXIT_SUCCESS)
    {
      ret = process();
    }

  return ret;
}
