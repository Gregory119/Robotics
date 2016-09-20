#include "core_servo.h"
#include "utl_mapping.h"
#include <wiringPi.h>
#include <assert.h>

static const unsigned s_max_8bit = 255;
static const unsigned s_min_8bit = 0;

using namespace CORE;

//----------------------------------------------------------------------//
Servo::Servo(unsigned control_pin)
  : d_pin(control_pin),
    d_pos_8bit_to_us(UTIL::Map(s_max_8bit, s_min_8bit, d_max_us, d_min_us))
{
  initPins();
}

//----------------------------------------------------------------------//
Servo::Servo(const Servo& copy)
  : d_pos_8bit_to_us(UTIL::Map(s_max_8bit, s_min_8bit, d_max_us, d_min_us))
{
  operator=(copy);
  initPins();
}

//----------------------------------------------------------------------//
Servo& Servo::operator=(const Servo& copy)
{
  //the same settings
  d_pin = copy.d_pin;
  d_min_us = copy.d_min_us;
  d_max_us = copy.d_max_us;
  d_delay_us = copy.d_delay_us;
  //not the same position

  return *this;
}

//----------------------------------------------------------------------//
void Servo::setup()
{
  wiringPiSetup();
}

//----------------------------------------------------------------------//
void Servo::initPins()
{
  pinMode(d_pin, OUTPUT);
  digitalWrite(d_pin, LOW);
}

//----------------------------------------------------------------------//
Servo::~Servo()
{
  stop();
}

//----------------------------------------------------------------------//
void Servo::setTiming(unsigned min_us,
		      unsigned max_us,
		      unsigned delay_us)
{
  d_min_us = min_us;
  d_max_us = max_us;
  d_delay_us = delay_us;
  d_pos_8bit_to_us = UTIL::Map(s_max_8bit, s_min_8bit, d_max_us, d_min_us);
}

//----------------------------------------------------------------------//
void Servo::setPos(uint8_t pos)
{
  std::lock_guard<std::mutex> lock(m);
  d_pos = pos;
}

//This needs to be placed in a thread in some way
//----------------------------------------------------------------------//
void Servo::updatePos()
{
  std::lock_guard<std::mutex> lock(m);
  unsigned pos_us = mapFromTo(d_pos_8bit_to_us, d_pos);

  digitalWrite(d_pin, HIGH);
  delayMicroseconds(pos_us);
  digitalWrite(d_pin, LOW);
  delayMicroseconds(d_delay_us);
}

//----------------------------------------------------------------------//
bool Servo::incrementPos(uint8_t pos)
{
  std::lock_guard<std::mutex> lock(m);
  if ((pos + d_pos) > s_max_8bit)
    {
      d_pos = s_max_8bit;
      return false;
    }
  else
    {
      d_pos += pos;
    }
  return true;
}

//----------------------------------------------------------------------//
bool Servo::decrementPos(uint8_t pos)
{
  std::lock_guard<std::mutex> lock(m);
  if (pos > d_pos)
    {
      d_pos = s_min_8bit;
      return false;
    }
  else
    {
      d_pos -= pos;
    }
  return true;  
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
void Servo::stop()
{
  if (!d_running)
    return;
    
  d_thread_shutdown.set_value(true); //stop the thread
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
//Servo::
