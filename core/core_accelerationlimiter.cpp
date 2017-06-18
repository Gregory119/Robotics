#include "core_accelerationlimiter.h"

using namespace CORE;

//----------------------------------------------------------------------//
AccelerationLimiter::AccelerationLimiter(unsigned max_speed, 
					 unsigned min_time_for_max_speed,
					 const AccelerationLimiter::SpeedOutputCallbackFunc& f)
  : d_max_speed(max_speed),
    d_min_time_for_max_speed(min_time_for_max_speed),
    d_call_back(f)
{
}

//----------------------------------------------------------------------//
void AccelerationLimiter::setTimeIncrement(unsigned time_ms)
{
  d_time_inc_ms = time_ms;
}

//----------------------------------------------------------------------//
void AccelerationLimiter::requestSpeed(int set_speed)
{
  d_set_speed = set_speed;
  updateValues();
}

//----------------------------------------------------------------------//
void AccelerationLimiter::updateValues()
{
  // if time increment has passed since last increment
  //   calculate speed increment
  //   update output speed
  //   update current speed
  //   call the call-back function with new output speed
}
