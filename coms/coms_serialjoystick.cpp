#include "coms_serialjoystick.h"
#include "utl_mapping.h"

#include <wiringSerial.h>
#include <iostream>
#include <cmath>
#include <unistd.h>

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

  d_desc = serialOpen(serial_port, baud);

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
      serialClose(d_desc);
    }
}

//----------------------------------------------------------------------//
void JoystickTransmitter::handleEvent(const JS::JSEvent &event) 
{
  std::lock_guard<std::mutex> lock(m);
  d_serial_cmd = "J";
  d_serial_cmd += convertValueTo8Bits(event.time, s_u32_max_digits);
  d_serial_cmd += convertValueTo8Bits(event.value, s_s16_max_digits);
  d_serial_cmd += convertValueTo8Bits(event.type, s_u8_max_digits);
  d_serial_cmd += convertValueTo8Bits(event.number, s_u8_max_digits);
  d_serial_cmd += "#";
  
  std::cout<<"serial: " << d_serial_cmd << std::endl;
  std::cout << "time [ms]: " << (int)event.time << std::endl;
  std::cout << "value: " << (int)event.value << std::endl;
  std::cout << "type: " << (int)event.type << std::endl;
  std::cout << "number: " << (int)event.number << std::endl;
  serialPuts(d_desc, d_serial_cmd.c_str());
}

//----------------------------------------------------------------------//
bool JoystickTransmitter::stayRunning()
{
  return d_stay_running;
}

//----------------------------------------------------------------------//
void JoystickTransmitter::handleReadError()
{
  std::lock_guard<std::mutex> lock(m);
  d_stay_running = false;
  d_js->stop();
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
bool JoystickReceiver::init(const char* serial_port,
			    int baud)
{
  d_desc = serialOpen(serial_port, baud);
  if (d_desc == -1)
    {
      std::cout << "Could not open serial port" << std::endl;
      return false;
    }

  return true;
}

//----------------------------------------------------------------------//
bool JoystickReceiver::readSerialEvent(JS::JSEventMinimal &js_event)
{
  //this algorithm will go through every character received to check for an event
  if (serialDataAvail(d_desc) < s_js_serial_chars)
    {
      return false;
    }
 
    int next_char = serialGetchar(d_desc);

  if (next_char != 'J')
    {
      std::cout<<"no serial joystick start character"<<std::endl;
      return false;
    }

  //need to decompress the time value from a compressed uint8_t to a uint16_t
  d_js_event.time_ms = mapFromTo(s_uchar_time_to_uint16_map, serialGetchar(d_desc));
  d_js_event.value = serialGetchar(d_desc);
  d_js_event.type = serialGetchar(d_desc);
  d_js_event.number = serialGetchar(d_desc);
  
  if (serialGetchar(d_desc) != '#')
    {
      std::cout<<"no serial joystick end character"<<std::endl;
      return false;
    }
  js_event = d_js_event;

  return true;
}

//----------------------------------------------------------------------//
//JoystickReceiver::
