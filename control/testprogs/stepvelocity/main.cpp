#include "ctrl_rcstepvelocitymanager.h"

#include "kn_stdkernel.h"
#include "kn_timer.h"

#include <iostream>

#include <memory>

class TestTimers : KERN::KernelTimerOwner
{
public:
  TestTimers()
    : d_timer(new KERN::KernelTimer(this)),
      d_input_timer(new KERN::KernelTimer(this))
  {
    CTRL::RCStepVelocityManager::VelocityLimitParams params;
    params.abs_max_velocity = 255;
    params.min_time_for_max_velocity_ms = 10000;
    params.time_step_ms = 500;
    
    d_man.reset(new CTRL::RCStepVelocityManager(params));
    d_timer->restartMs(params.time_step_ms);

    d_input_timer->restartMs(7500);
  }

private:
  bool handleTimeOut(const KERN::KernelTimer& timer)
  {
    if (timer==d_timer.get())
      {
	std::cout << "Requested velocity = " << d_req_velocity << std::endl;
	std::cout << "New output velocity = " << d_man->stepVelocity(d_req_velocity) << std::endl;
	if (d_input_timer->isDisabled())
	  {
	    d_timer->disable();
	  }
	return true;
      }

    if (timer==d_input_timer.get())
      {
	if (d_input_timer->getConseqTimeOuts() == 1)
	  {
	    d_req_velocity = 0;
	  }
	else if (d_input_timer->getConseqTimeOuts() == 2)
	  {
	    d_req_velocity = 120;
	  }
	else if (d_input_timer->getConseqTimeOuts() == 3)
	  {
	    d_req_velocity = -120;
	  }
	else if (d_input_timer->getConseqTimeOuts() == 4)
	  {
	    d_req_velocity = 0;
	  }
	else if (d_input_timer->getConseqTimeOuts() == 5)
	  {
	    d_req_velocity = -120;
	  }
	else if (d_input_timer->getConseqTimeOuts() == 6)
	  {
	    d_req_velocity = 120;
	  }
	else
	  {
	    d_input_timer->disable();
	  }
	return true;
      }

    return false;
  }

private:
  int d_req_velocity = 120;
  
  std::unique_ptr<KERN::KernelTimer> d_timer;
  std::unique_ptr<KERN::KernelTimer> d_input_timer;
  std::unique_ptr<CTRL::RCStepVelocityManager> d_man;
};

int main(int argc, char* argv[])
{
  KERN::StdKernel k;
  
  TestTimers test_timers;
  
  return k.run();
}
