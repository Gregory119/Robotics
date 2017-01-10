#include "core_servo.h"

#include <assert.h>

using namespace CORE;

static const unsigned s_max_8bit = 255;
static const unsigned s_min_8bit = 0;

//----------------------------------------------------------------------//
Servo::Servo()
  : d_pos_8bit_to_pulse(UTIL::Map(s_max_8bit, s_min_8bit, d_max_pulse, d_min_pulse))
{}

//----------------------------------------------------------------------//
Servo::~Servo()
{
 stop()
}

//----------------------------------------------------------------------//
Servo::Servo(const Servo& copy)
  : Servo()
{
  operator=(copy);
}

//----------------------------------------------------------------------//
Servo& Servo::operator=(const Servo& copy)
{
  d_min_pulse = copy.d_min_pulse;
  d_max_pulse = copy.d_max_pulse;
  d_delay_us = copy.d_delay_us;
  //not the same position

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
void Servo::setDelayTimeUs(unsigned delay_us)
{
  d_delay_us = delay_us;
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
void Servo::run()
{
  assert(!d_running);
  
  d_running = true;
  std::thread t(&threadFunc,
		d_thread_shutdown.get_future(),
		this);
  t.detach();
}

//----------------------------------------------------------------------//
void Servo::threadFunc(std::future<bool> shutdown,
			   Servo *const servo)
{
  while (shutdown.wait_for(std::chrono::nanoseconds(0)) != 
	 std::future_status::ready)
    {
      servo->updatePos();
    }
}

//----------------------------------------------------------------------//
void Servo::stop()
{
  if (!d_running)
    return;
    
  d_thread_shutdown.set_value(true); //stop the thread
}

//----------------------------------------------------------------------//
void Servo::moveToPos(uint8_t pos)
{
  std::lock_guard<std::mutex> lock(d_m);
  setPosValue(pos);
}

//----------------------------------------------------------------------//
bool Servo::incrementMove(uint8_t pos)
{
  std::lock_guard<std::mutex> lock(d_m);
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
  std::lock_guard<std::mutex> lock(d_m);
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
