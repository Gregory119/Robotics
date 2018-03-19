#include "crc_pwmutils.h"

#include <cassert>
#include <type_traits>

using namespace C_RC;

//----------------------------------------------------------------------//
const std::chrono::microseconds& getMaxPwmDuration()
{
  static std::chrono::microseconds s_max_pwm_pulse_duration(2500);
  return s_max_pwm_pulse_duration;
}

//----------------------------------------------------------------------//
const std::chrono::microseconds& getMinPwmDuration()
{
  static const std::chrono::microseconds s_min_pwm_pulse_duration(500);
  return s_min_pwm_pulse_duration;
}
