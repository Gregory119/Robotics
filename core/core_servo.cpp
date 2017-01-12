#include "core_servo.h"

#include <assert.h>

using namespace CORE;

static const unsigned s_max_8bit = 255;
static const unsigned s_min_8bit = 0;

//----------------------------------------------------------------------//
Servo::Servo()
  : d_pos_8bit_to_pulse(UTIL::Map(s_max_8bit, s_min_8bit, d_max_pulse, d_min_pulse))
{
  run();
}

//----------------------------------------------------------------------//
Servo::~Servo()
{
  stop();
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
uint8_t Servo::getRangePos()
{
  return s_max_8bit - s_min_8bit;
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

//----------------------------------------------------------------------//
void Servo::setPosValue(uint8_t pos)
{ 
  d_pos = pos; 
}

//----------------------------------------------------------------------//
bool Servo::incrementMove(uint8_t pos)
{
  if (!isPosInRange(pos + getPos()))
    {
      //print a warning here
      setPosValue(getMaxPos());
      return false;
    }
  else
    {
      setPosValue(getPos()+pos);
    }
  return true;
}

//----------------------------------------------------------------------//
bool Servo::decrementMove(uint8_t pos)
{
  if (!isPosInRange(getPos()-pos))
    {
      //print a warning here
      setPosValue(getMinPos());
      return false;
    }
  else
    {
      setPosValue(getPos()-pos);
    }
  return true;  
}
