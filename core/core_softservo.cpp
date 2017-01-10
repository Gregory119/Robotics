#include "core_softservo.h"
#include "utl_mapping.h"

#include <wiringPi.h>

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
void SoftServo::updatePos()
{
  getMutex().lock();
  unsigned pos = getPos();
  getMutex().unlock();
  unsigned pos_us = UTIL::mapFromTo(getPosMap(), pos);

  unsigned time_us = micros();
  digitalWrite(d_pin, HIGH);
  while((micros()-time_us)<pos_us){}
  digitalWrite(d_pin, LOW);
  delayMicroseconds(d_delay_us);
}

//----------------------------------------------------------------------//
//SoftServo::
