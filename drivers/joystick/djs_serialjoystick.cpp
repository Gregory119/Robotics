#include "djs_serialjoystick.h"

#include <wiringSerial.h>
#include <iostream>
#include <cmath>
#include <unistd.h>

using namespace D_JS;

const unsigned char s_js_serial_chars = 6;

const unsigned JoystickTransmitter::s_u32_max_digits = 10;
const unsigned JoystickTransmitter::s_s16_max_digits = 5;
const unsigned JoystickTransmitter::s_u8_max_digits = 3;

const unsigned JoystickTransmitter::s_max_time_ms = 3000;
const UTIL::Map JoystickTransmitter::s_time_to_uchar_map(s_max_time_ms,0,255,0);
const UTIL::Map JoystickTransmitter::s_value_to_char_map(32767,-32767,255,0);

//----------------------------------------------------------------------//
// JoystickTransmitter
//----------------------------------------------------------------------//
JoystickTransmitter::JoystickTransmitter()
  : d_resend_timer(KERN::KernelTimer(this))
{}

//----------------------------------------------------------------------//
bool JoystickTransmitter::init(const std::string& serial_port,
			       int baud,
			       const std::string& js_source)
{
  d_js.reset(new JoyStick(this,js_source.c_str()));

  if (!d_js->init())
    {
      std::cout << "Joystick init failed" << std::endl;
      return false;
    }

  d_desc = serialOpen(serial_port.c_str(), baud);

  if (d_desc == -1)
    {
      std::cout << "Could not open serial port" << std::endl;
      return false;
    }

  d_is_init = true;
  
  return true;
}

//----------------------------------------------------------------------//
void JoystickTransmitter::start()
{
  // initially made large to avoid holding up other timeouts
  d_resend_timer.restartMs(d_resend_time_ms);
  assert(d_is_init);
  if (d_is_init)
    {
      d_js->run();
    }
}

//----------------------------------------------------------------------//
JoystickTransmitter::~JoystickTransmitter()
{
  if (d_desc != -1)
    {
      serialClose(d_desc);
    }
}

//-----------------------------------------------------------------------/
void JoystickTransmitter::setResendEventTimeoutMs(long t_ms)
{
  std::lock_guard<std::mutex> lock(d_m);
  d_enable_resend_event = true;
  d_resend_time_ms = t_ms;
}

//-----------------------------------------------------------------------/
void JoystickTransmitter::resendLastEvent()
{
  std::lock_guard<std::mutex> lock(d_m);
  assert(d_enable_resend_event);
  sendEvent(d_resend_event);
}

//----------------------------------------------------------------------//
void JoystickTransmitter::handleEvent(const JSEvent &event) 
{
  std::lock_guard<std::mutex> lock(d_m);
  if (d_resend_only_axis && event.type == D_JS::AXIS)
    {
      d_resend_event = event; // update event to be resent with only axis type values
    }
  sendEvent(event);
}

//----------------------------------------------------------------------//
void JoystickTransmitter::sendEvent(const JSEvent &event)
{
  std::cout << "sendEvent" << std::endl;
  d_resend_timer.restart(); // timer should timeout when no event has been received
  
  serialPutchar(d_desc, 'J');
  // does not send the time
  if (event.type == BUTTON)
    {
      serialPutchar(d_desc, mapToChar(event.value, s_u8_max_digits));
    }
  else if (event.type == AXIS)
    {
      serialPutchar(d_desc, mapToChar(event.value, s_s16_max_digits));
    }
  serialPutchar(d_desc, mapToChar(event.type, s_u8_max_digits));
  serialPutchar(d_desc, mapToChar(event.number, s_u8_max_digits));
  serialPutchar(d_desc, '#');
  /*
    std::cout << "=========================================" << std::endl;
    std::cout << "time [ms]: " << (int)event.time << std::endl;
    std::cout << "value: " << (int)event.value << std::endl;
    std::cout << "type: " << (int)event.type << std::endl;
    std::cout << "number: " << (int)event.number << std::endl;

    std::cout << "time [8bits]: " << (int)mapToChar(event.time, s_u32_max_digits) << std::endl;
  
    if (event.type == BUTTON)
    {
    std::cout << "value [8bits]: " << (int)mapToChar(event.value, s_u8_max_digits) << std::endl;
    }
    else if (event.type == AXIS)
    {
    std::cout << "value [8bits]: " << (int)mapToChar(event.value, s_s16_max_digits) << std::endl;
    }
    std::cout << "type [8bits]: " << (int)mapToChar(event.type, s_u8_max_digits) << std::endl;
    std::cout << "number [8bits]: " << (int)mapToChar(event.number, s_u8_max_digits) << std::endl;
  */
}

//----------------------------------------------------------------------//
void JoystickTransmitter::handleReadError()
{
  std::lock_guard<std::mutex> lock(d_m);
  d_js->stop();
}

//----------------------------------------------------------------------//
bool JoystickTransmitter::handleTimeOut(const KERN::KernelTimer& timer)
{
  if (timer.is(d_resend_timer))
    {
      if (d_enable_resend_event)
				{
					resendLastEvent();
				}
      // else
      // do nothing
      // only used to keep running in order to process the threaded joystick events
      return true;
    }
  return false;
}

//----------------------------------------------------------------------//
void JoystickTransmitter::enableOnlyResendAxisEvents(bool state)
{
  d_resend_only_axis = state;
}

//----------------------------------------------------------------------//
// JoystickReceiver
//----------------------------------------------------------------------//

JoystickReceiver::JoystickReceiver()
{}

//----------------------------------------------------------------------//
JoystickReceiver::~JoystickReceiver()
{
  if (d_desc != -1)
    {
      serialClose(d_desc);
    }
}

//----------------------------------------------------------------------//
bool JoystickReceiver::init(const std::string& serial_port,
			    int baud)
{
  d_desc = serialOpen(serial_port.c_str(), baud);
  if (d_desc == -1)
    {
      std::cout << "Could not open serial port" << std::endl;
      return false;
    }

  return true;
}

//----------------------------------------------------------------------//
bool JoystickReceiver::readSerialEvent(JSEventMinimal &js_event)
{
  //this algorithm will go through every character received to check for an event
  
  int num_chars = serialDataAvail(d_desc);
  if (num_chars < s_js_serial_chars)
    {
      return false;
    }
 
  int next_char = serialGetchar(d_desc);
  if (next_char < 0)
    {
      return false;
    }

  if (next_char != 'J')
    {
      std::cout<<"no serial joystick start character"<<std::endl;
      return false;
    }

  //need to decompress the time value from a compressed uint8_t to a uint16_t
  d_js_event.value = serialGetchar(d_desc);
  d_js_event.type = serialGetchar(d_desc);
  d_js_event.number = serialGetchar(d_desc);
  
  if (serialGetchar(d_desc) != '#')
    {
      std::cout<<"no serial joystick end character"<<std::endl;
      serialFlush(d_desc);
      return false;
    }
  js_event = d_js_event;

  //std::cout << "time [ms]: " << (int)d_js_event.time_ms << std::endl;
  std::cout << "value: " << (int)d_js_event.value << std::endl;
  //std::cout << "type: " << (int)d_js_event.type << std::endl;
  //std::cout << "number: " << (int)d_js_event.number << std::endl;

  return true;
}
