#include "ctrl_rcstepvelocitymanager.h"

#include "kn_asiokernel.h"
#include "kn_asiocallbacktimer.h"

#include <chrono>
#include <iostream>
#include <memory>

class TestTimers
{
public:
  TestTimers()
  {
    CTRL::RCStepVelocityManager::VelocityLimitParams params;
    params.abs_max_velocity = 255;
    params.min_time_for_max_velocity_ms = 10000;
    params.time_step_ms = 500;
    
    d_man.reset(new CTRL::RCStepVelocityManager(params));
    d_timer.setTimeoutCallback([this](){
	processTimer();
      });
    d_timer.restart(std::chrono::milliseconds(params.time_step_ms));

    d_input_timer.setTimeoutCallback([this](){
	processInputTimer();
      });
    d_input_timer.restart(std::chrono::milliseconds(7500));
  }

private:
  void processTimer()
  {
    std::cout << "Requested velocity = " << d_req_velocity << std::endl;
    int vel = d_man->stepToVelocity(d_req_velocity);
    std::cout << "New output velocity = " << vel << std::endl;
    if (d_input_timer.isDisabled())
      {
	d_timer.disable();
      }
  }

  void processInputTimer()
  {
    if (d_input_timer.getConseqTimeOuts() == 1)
      {
	d_req_velocity = 0;
      }
    else if (d_input_timer.getConseqTimeOuts() == 2)
      {
	d_req_velocity = 120;
      }
    else if (d_input_timer.getConseqTimeOuts() == 3)
      {
	d_req_velocity = -120;
      }
    else if (d_input_timer.getConseqTimeOuts() == 4)
      {
	d_req_velocity = 0;
      }
    else if (d_input_timer.getConseqTimeOuts() == 5)
      {
	d_req_velocity = -120;
      }
    else if (d_input_timer.getConseqTimeOuts() == 6)
      {
	d_req_velocity = 120;
      }
    else
      {
	d_input_timer.disable();
      }
  }

private:
  int d_req_velocity = 120;
  
  KERN::AsioCallbackTimer d_timer = KERN::AsioCallbackTimer("Test timer.");
  KERN::AsioCallbackTimer d_input_timer = KERN::AsioCallbackTimer("Test input timer.");;
  std::unique_ptr<CTRL::RCStepVelocityManager> d_man;
};

int main(int argc, char* argv[])
{
  KERN::AsioKernel k;
  
  TestTimers test_timers;
  
  k.run();
  return 0;
}
