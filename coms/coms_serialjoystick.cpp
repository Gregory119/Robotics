//#define DEBUG

#include "coms_serialjoystick.h"
#include "utl_mapping.h"

#ifndef DEBUG
#include <wiringSerial.h>
#endif

#include <cmath>
#include <iostream>

using namespace COMS;

const unsigned char s_js_serial_chars = 6;

const unsigned JoystickTransmitter::s_u32_max_digits = 10;
const unsigned JoystickTransmitter::s_s16_max_digits = 5;
const unsigned JoystickTransmitter::s_u8_max_digits = 3;

const unsigned JoystickTransmitter::s_max_time_ms = 3000;
const UTIL::Map JoystickTransmitter::s_time_to_uchar_map(s_max_time_ms,0,255,0);
const UTIL::Map JoystickTransmitter::s_value_to_char_map(32767,-32767,127,-127);

const UTIL::Map JoystickReceiver::s_uchar_time_to_uint16_map(255,0,JoystickTransmitter::s_max_time_ms,0);

//----------------------------------------------------------------------//
// JoystickTransmitter
//----------------------------------------------------------------------//

JoystickTransmitter::JoystickTransmitter() 
{}

//----------------------------------------------------------------------//
bool JoystickTransmitter::init(const char* serial_port,
			       int baud,
			       const char* js_source)
{
  d_js.reset(new JS::JoyStick(this,js_source));

  if (!d_js->init())
    {
      std::cout << "Joystick init failed" << std::endl;
      d_stay_running = false;
      return false;
    }
#ifndef DEBUG
  d_desc = serialOpen(serial_port, baud);
#endif
  if (d_desc == -1)
    {
      std::cout << "Could not open serial port" << std::endl;
      d_stay_running = false;
      return false;
    }

  d_js->run();

  return true;
}

//----------------------------------------------------------------------//
JoystickTransmitter::~JoystickTransmitter()
{
  if (d_desc != -1)
    {
      #ifndef DEBUG
      serialClose(d_desc);
      #endif
    }
}

//----------------------------------------------------------------------//
void JoystickTransmitter::handleEvent(const JS::JSEvent &event) 
{
  d_serial_cmd = "J";
  d_serial_cmd += convertValueTo8Bits(event.time, s_u32_max_digits);
  d_serial_cmd += convertValueTo8Bits(event.value, s_s16_max_digits);
  d_serial_cmd += convertValueTo8Bits(event.type, s_u8_max_digits);
  d_serial_cmd += convertValueTo8Bits(event.number, s_u8_max_digits);
  d_serial_cmd += "#";
  
  std::cout<<"serial: " << d_serial_cmd << std::endl;
#ifndef DEBUG
  serialPuts(d_desc, d_serial_cmd.c_str());
#endif
}

//----------------------------------------------------------------------//
bool JoystickTransmitter::stayRunning()
{
  return d_stay_running;
}

//----------------------------------------------------------------------//
void JoystickTransmitter::handleReadError()
{
  d_stay_running = false;
  d_js->stop();
}

//----------------------------------------------------------------------//
// JoystickReceiver
//----------------------------------------------------------------------//

JoystickReceiver::~JoystickReceiver()
{
  if (d_desc != -1)
    {
      #ifndef DEBUG
      serialClose(d_desc);
      #endif
    }
}

//----------------------------------------------------------------------//
bool JoystickReceiver::init(const char* serial_port,
			    int baud)
{
  #ifndef DEBUG
  d_desc = serialOpen(serial_port, baud);
  #endif
  if (d_desc == -1)
    {
      std::cout << "Could not open serial port" << std::endl;
      return false;
    }

  return true;
}

//----------------------------------------------------------------------//
bool JoystickReceiver::flushInvalidSerialEvent()
{
#ifndef DEBUG
  if (serialDataAvail(d_desc) >= s_js_serial_chars)
    {
      for (unsigned i=0; i<s_js_serial_chars; ++i)
	{
	  serialGetchar(d_desc);	  
	}

      return true;
      }
#endif
  return false;
}

//----------------------------------------------------------------------//
bool JoystickReceiver::readSerialEvent(JS::JSEventMinimal &js_event)
{
#ifndef DEBUG
  if (serialDataAvail(d_desc) < s_js_serial_chars)
    {
      return false;
    }
 
    int next_char = serialGetchar(d_desc);

  if (next_char != 'J')
    {
      std::cout<<"no serial joystick start character"<<std::endl;
      flushInvalidSerialEvent();
      return false;
    }

  char char_time = serialGetchar(d_desc);
  uint16_t temp_time = (uint16_t)char_time;
  std::cout<<"char_time: "<<(int)char_time<<std::endl;
  std::cout<<"temp_time: "<<(int)temp_time<<std::endl;
  d_js_event.time_ms = mapFromTo(s_uchar_time_to_uint16_map, temp_time);
  d_js_event.value = serialGetchar(d_desc);
  d_js_event.type = serialGetchar(d_desc);
  d_js_event.number = serialGetchar(d_desc);
  
  if (serialGetchar(d_desc) != '#')
    {
      std::cout<<"no serial joystick end character"<<std::endl;
      flushInvalidSerialEvent();
      return false;
    }
#endif
  js_event = d_js_event;

  return true;
}

//----------------------------------------------------------------------//
//JoystickReceiver::
