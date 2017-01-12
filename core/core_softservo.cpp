#include "core_softservo.h"
#include "utl_mapping.h"

#include <wiringPi.h>

#include <unistd.h>

using namespace CORE;

//----------------------------------------------------------------------//
SoftServo::SoftServo(unsigned control_pin)
  : d_pin(control_pin)
{
  initPins();
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
    
  d_running=false;
  d_thread_shutdown.set_value(true); //stop the thread
}

//----------------------------------------------------------------------//
void SoftServo::updatePos()
{
  unsigned pos = getPos();
  unsigned pos_us = UTIL::mapFromTo(getPosMap(), pos);

  unsigned time_us = micros();
  digitalWrite(d_pin, HIGH);
  while((micros()-time_us)<pos_us){}
  digitalWrite(d_pin, LOW);
  usleep(getDelayTimeUs());
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
unsigned SoftServo::getPos() const
{ 
  std::lock_guard<std::mutex> lock(d_m);
  return Servo::getPos();
}

//----------------------------------------------------------------------//
void SoftServo::setDelayTimeUs(unsigned delay_us)
{
  std::lock_guard<std::mutex> lock(d_m);
  d_delay_us = delay_us;
}

//----------------------------------------------------------------------//
unsigned SoftServo::getDelayTimeUs()
{
  std::lock_guard<std::mutex> lock(d_m);
  return d_delay_us;
}

//----------------------------------------------------------------------//
void SoftServo::moveToPos(uint8_t pos)
{
  setPosValue(pos);
}

//----------------------------------------------------------------------//
const UTIL::Map& SoftServo::getPosMap() const
{
  std::lock_guard<std::mutex> lock(d_m);
  return Servo::getPosMap();
}

//----------------------------------------------------------------------//
unsigned SoftServo::getPulseMinTimeUs() const
{
  std::lock_guard<std::mutex> lock(d_m);
  return Servo::getPulseMinTimeUs();
}

//----------------------------------------------------------------------//
unsigned SoftServo::getPulseMaxTimeUs() const
{
  std::lock_guard<std::mutex> lock(d_m);
  return Servo::getPulseMaxTimeUs();
}

//----------------------------------------------------------------------//
void SoftServo::setTiming(unsigned min_pulse,
			  unsigned max_pulse)
{
  std::lock_guard<std::mutex> lock(d_m);
  Servo::setTiming(min_pulse, max_pulse)
}

//----------------------------------------------------------------------//
void SoftServo::setPosValue(uint8_t pos) 
{
  std::lock_guard<std::mutex> lock(d_m);
  Servo::setPosValue(pos);
}

//----------------------------------------------------------------------//
//SoftServo::
