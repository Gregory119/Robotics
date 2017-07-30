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
  // init pins
  pinMode(d_pin, OUTPUT);
  digitalWrite(d_pin, LOW);
}

//----------------------------------------------------------------------//
void SoftServo::setup()
{
  wiringPiSetup();
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
  std::lock_guard<std::mutex> lock(d_m);
  unsigned pos_us = getSetPulseUs();
  unsigned time_us = micros();
  digitalWrite(d_pin, HIGH);
  while((micros()-time_us)<pos_us){}
  digitalWrite(d_pin, LOW);
  usleep(getDelayTimeUs());
}

//----------------------------------------------------------------------//
void SoftServo::threadFunc(std::future<bool> shutdown,
			   SoftServo *const servo) // change this in the future to use a thread safe shared pipe reader/writer
{
  while (shutdown.wait_for(std::chrono::nanoseconds(0)) != 
	 std::future_status::ready)
    {
      servo->updateMove();
    }
}

//----------------------------------------------------------------------//
void SoftServo::moveToPos(int pos)
{
  assert(isPosValid(pos));
  
  if (!d_first_move)
    {
      setReqPos(pos);
    }
  else
    {
      moveToStartPos(pos); // first move sets the set position to avoid the increment manager
      d_first_move = false;
    }
}

//----------------------------------------------------------------------//
unsigned SoftServo::getDelayTimeUs()
{
  std::lock_guard<std::mutex> lock(d_m);
  return d_delay_us;
}
