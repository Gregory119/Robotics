#ifndef CORE_ACCELERATIONLIMITER_H
#define CORE_ACCELERATIONLIMITER_H

#include <functional>

//input derivative limiters eg. velocity derivative (acceleration) limiter

namespace CORE
{
  // need some sort of kernel
  class AccelerationLimiter
  {
  public:
    using SpeedOutputCallbackFunc = std::function<void(unsigned)>;
    
    // any speed units, but keep them consistent
    AccelerationLimiter(unsigned max_speed, 
			unsigned min_time_for_max_speed,
			const SpeedOutputCallbackFunc& f);

    void setTimeIncrement(unsigned time_ms); // values are updated every time increment

    void requestSpeed(int set_speed);
    // This must be called frequently in order to update the output speed
    // Input speed sign implies direction. Output control speed will increase linearly and be limited to a max acceleration. The current speed is assumed to be the set output speed on every time increment update.

  private:
    void updateValues();
    
  private:
    unsigned d_max_speed = 0;
    unsigned d_min_time_for_max_speed = 0;
    unsigned d_time_inc_ms = 5;
    int d_set_speed = 0;
    int d_current_speed = 0;
    int d_output_speed = 0;
    SpeedOutputCallbackFunc d_call_back;
  };
};

#endif
