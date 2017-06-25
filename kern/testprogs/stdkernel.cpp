#include "kn_stdkernel.h"
#include "kn_timer.h"

#include <iostream>
#include <memory>

static const int s_num_allowed_timer1_timeouts = 2;
static const int s_num_allowed_timer2_timeouts = 3;

class TestTimers : KERN::KernelTimerOwner
{
public:
  TestTimers()
  {
    d_timer1.reset(new KERN::KernelTimer(this));
    d_timer2.reset(new KERN::KernelTimer(this));
    d_timer3.reset(new KERN::KernelTimer(this));
    d_timer1->restartMs(3000);
    std::cout << "started timer 1" << std::endl;
  }

private:
  bool handleTimeOut(const KERN::KernelTimer& timer)
  {
    if (timer==d_timer1.get())
      {
	std::cout << "timer 1 has timed out with a consequetive count of " << d_timer1->getConseqTimeOuts() << ", a total count of " << d_timer1->getTotalTimeOuts() << std::endl;

	if (s_num_allowed_timer1_timeouts <= d_timer1->getConseqTimeOuts())
	  {
	    std::cout << "Disabling timer 1 and starting timer 2" << std::endl;
	    d_timer1->disable();
	    d_timer2->restartMs(2000);
	  }
	return true;
      }
    else if (timer==d_timer2.get())
      {
	std::cout << "timer 2 has timed out with a consequetive count of " << d_timer2->getConseqTimeOuts() << ", a total count of " << d_timer2->getTotalTimeOuts() << std::endl;

	if (s_num_allowed_timer2_timeouts > d_timer2->getTotalTimeOuts())
	  {
	    std::cout << "Disabling timer 2 and restarting timer 1" << std::endl;
	    d_timer2->disable();
	    d_timer1->restart();
	  }
	else
	  {
	    std::cout << "Disabling timer 2 and starting timer 3." << std::endl;
	    d_timer2->disable();
	    d_timer3->restartMsIfNotSet(1000);
	  }
	
	return true;
      }
    else if (timer==d_timer3.get())
      {
	std::cout << "timer 3 has timed out with a consequetive count of " << d_timer3->getConseqTimeOuts() << ", a total count of " << d_timer3->getTotalTimeOuts() << std::endl;
	std::cout << "Disabling timer 3 now." << std::endl;
	d_timer3->disable();
	
	return true;
      }

    return false;
  }

private:
  std::unique_ptr<KERN::KernelTimer> d_timer1;
  std::unique_ptr<KERN::KernelTimer> d_timer2;
  std::unique_ptr<KERN::KernelTimer> d_timer3;
};

int main(int argc, char** argv)
{
  KERN::StdKernel k;
  
  TestTimers test_timers;
  
  return k.run();
}
