#include "core_softservo.h"
#include "utl_mapping.h"

#include <wiringPi.h>
#include <assert.h>

using namespace CORE;

//----------------------------------------------------------------------//
SoftServo::SoftServo(unsigned control_pin)
  : d_pin(control_pin)
{
  initPins();
}

//----------------------------------------------------------------------//
SoftServo::~SoftServo()
{
  stop();
}

//----------------------------------------------------------------------//
SoftServo::SoftServo(const SoftServo& copy)
  : Servo(copy)
{
  operator=(copy);
  initPins();
}

//----------------------------------------------------------------------//
SoftServo& SoftServo::operator=(const SoftServo& copy)
{
  Servo::operator=(copy);
  //the same settings
  d_pin = copy.d_pin;
  d_delay_us = copy.d_delay_us;
  //not the same position

  return *this;
}

//----------------------------------------------------------------------//
void SoftServo::setup()
{
  wiringPiSetup();
}

//----------------------------------------------------------------------//
void SoftServo::initPins()
{
  pinMode(d_pin, OUTPUT);
  digitalWrite(d_pin, LOW);
}

//----------------------------------------------------------------------//
void SoftServo::setDelayTimeUs(unsigned delay_us)
{
  d_delay_us = delay_us;
}

//----------------------------------------------------------------------//
void SoftServo::moveToPos(uint8_t pos)
{
  std::lock_guard<std::mutex> lock(d_m);
  setPosValue(pos);
}

//----------------------------------------------------------------------//
void SoftServo::updatePos()
{
  d_m.lock();
  unsigned pos = getPos();
  d_m.unlock();
  unsigned pos_us = UTIL::mapFromTo(getPosMap(), pos);

  unsigned time_us = micros();
  digitalWrite(d_pin, HIGH);
  while((micros()-time_us)<pos_us){}
  digitalWrite(d_pin, LOW);
  delayMicroseconds(d_delay_us);
}

//----------------------------------------------------------------------//
bool SoftServo::incrementMove(uint8_t pos)
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
bool SoftServo::decrementMove(uint8_t pos)
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

//----------------------------------------------------------------------//
void SoftServo::run()
{
  assert(!d_running);
  
  d_running = true;
  std::thread t(&threadFunc,
		d_thread_shutdown.get_future(),
		this);
  t.detach();
}

//----------------------------------------------------------------------//
void SoftServo::stop()
{
  if (!d_running)
    return;
    
  d_thread_shutdown.set_value(true); //stop the thread
}

//----------------------------------------------------------------------//
void SoftServo::threadFunc(std::future<bool> shutdown,
		       SoftServo *const servo)
{
  while (shutdown.wait_for(std::chrono::nanoseconds(0)) != 
	 std::future_status::ready)
    {
      servo->updatePos();
    }
}
//----------------------------------------------------------------------//
//SoftServo::
