#include "core_servo.h"

using namespace CORE;

static const unsigned s_max_8bit = 255;
static const unsigned s_min_8bit = 0;

//----------------------------------------------------------------------//
Servo::Servo()
  : d_pos_8bit_to_pulse(UTIL::Map(s_max_8bit, s_min_8bit, d_max_pulse, d_min_pulse))
{}

//----------------------------------------------------------------------//
Servo::Servo(const Servo& copy)
{
  operator=(copy);
}

//----------------------------------------------------------------------//
Servo& Servo::operator=(const Servo& copy)
{
  d_min_pulse = copy.d_min_pulse;
  d_max_pulse = copy.d_max_pulse;

  return *this;
}

//----------------------------------------------------------------------//
void Servo::setTiming(unsigned min_pulse,
		      unsigned max_pulse)
{
  d_min_pulse = min_pulse;
  d_max_pulse = max_pulse;
  d_pos_8bit_to_pulse = UTIL::Map(s_max_8bit, s_min_8bit, d_max_pulse, d_min_pulse);
}

//----------------------------------------------------------------------//
bool Servo::isPosInRange(uint8_t pos)
{
  if ((pos <= s_max_8bit) &&
      (pos >= s_min_8bit))
    {
      return true;
    }
  else
    {
      return false;
    }
}

//----------------------------------------------------------------------//
uint8_t Servo::getMaxPos()
{
  return s_max_8bit;
}

//----------------------------------------------------------------------//
uint8_t Servo::getMinPos()
{
  return s_min_8bit;
}
