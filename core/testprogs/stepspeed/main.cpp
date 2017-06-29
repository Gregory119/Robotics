#include "core_rcstepspeedmanager.h"

#include "kn_stdkernel.h"
#include "kn_timer.h"

#include <iostream>

#include <memory>

class TestTimers : KERN::KernelTimerOwner
{
public:
  TestTimers()
  {
    d_timer.reset(new KERN::KernelTimer(this));
    d_man.reset(new CORE::RCStepSpeedManager(???));
    d_timer->restartMs(800);
  }

private:
  bool handleTimeOut(const KERN::KernelTimer& timer)
  {
    if (timer==d_timer.get())
      {
	return true;
      }

    return false;
  }

private:
  std::unique_ptr<KERN::KernelTimer> d_timer;
  std::unique_ptr<CORE::RCStepSpeedManager> d_man;
};

int main(int argc, char* argv[])
{
  KERN::StdKernel k;
  
  TestTimers test_timers;
  
  return k.run();
}
