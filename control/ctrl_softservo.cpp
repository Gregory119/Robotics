#include "ctrl_softservo.h"
#include "utl_mapping.h"

#include <wiringPi.h>

#include <unistd.h>
#include <cassert>

using namespace CTRL;

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
  assert(!d_running);
    
  d_running=false;
  d_thread_shutdown.set_value(true); //stop the thread
}

//----------------------------------------------------------------------//
void SoftServo::updateMove()
{
  unsigned pos = getSetPos();
  unsigned pos_us = 0;
  UTIL::mapFromTo(getPosMap(), pos, pos_us);

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
      servo->updateMove();
    }
}

//----------------------------------------------------------------------//
unsigned SoftServo::getSetPos()
{ 
  std::lock_guard<std::mutex> lock(d_m);
  return Servo::getSetPos();
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
  if (!d_first_move)
    {
      setReqPos(pos);
    }
  else
    {
      moveToStartPos(pos);
      d_first_move = false;
    }
}

//----------------------------------------------------------------------//
const UTIL::Map& SoftServo::getPosMap()
{
  std::lock_guard<std::mutex> lock(d_m);
  return Servo::getPosMap();
}

//----------------------------------------------------------------------//
unsigned SoftServo::getPulseMinTimeUs()
{
  std::lock_guard<std::mutex> lock(d_m);
  return Servo::getPulseMinTimeUs();
}

//----------------------------------------------------------------------//
unsigned SoftServo::getPulseMaxTimeUs()
{
  std::lock_guard<std::mutex> lock(d_m);
  return Servo::getPulseMaxTimeUs();
}

//----------------------------------------------------------------------//
void SoftServo::setUsTiming(unsigned min_pulse,
			    unsigned max_pulse)
{
  std::lock_guard<std::mutex> lock(d_m);
  Servo::setUsTiming(min_pulse, max_pulse);
}

//----------------------------------------------------------------------//
void SoftServo::setSetPos(uint8_t pos)
{
  std::lock_guard<std::mutex> lock(d_m);
  Servo::setSetPos(pos);
}

//----------------------------------------------------------------------//
void SoftServo::setReqPos(uint8_t pos)
{
  std::lock_guard<std::mutex> lock(d_m);
  Servo::setReqPos(pos);
}

//----------------------------------------------------------------------//
uint8_t SoftServo::getReqPos()
{
  std::lock_guard<std::mutex> lock(d_m);
  return Servo::getReqPos();
}

//----------------------------------------------------------------------//
void SoftServo::setReqPosDirect(uint8_t pos)
{
  std::lock_guard<std::mutex> lock(d_m);
  Servo::setReqPosDirect(pos);
}

//----------------------------------------------------------------------//
//SoftServo::
