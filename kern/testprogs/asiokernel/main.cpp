#include "kn_asiokernel.h"
#include "kn_asiocallbacktimer.h"

#include <iostream>
#include <memory>

class TestTimers
{
public:
  TestTimers()
  {
    d_timer.setTimeoutCallback([](){
	std::cout << "Test Timer" << std::endl;
      });
    d_timer.restart(std::chrono::milliseconds(1000));
  }

private:
  KERN::AsioCallbackTimer d_timer;
};

int main(int argc, char** argv)
{
  KERN::AsioKernel k;
  
  TestTimers test_timers;
  
  k.run();
  return 0;
}
