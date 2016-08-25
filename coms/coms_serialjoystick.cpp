#include "coms_serialjoystick.h"
#include "utl_mapping.h"
#include <wiringSerial.h>
#include <cmath>
#include <iostream>

using namespace COMS;

const unsigned JoystickTransmitter::s_u32_max_digits = 10;
const unsigned JoystickTransmitter::s_s16_max_digits = 5;
const unsigned JoystickTransmitter::s_u8_max_digits = 3;

const unsigned JoystickTransmitter::s_max_time_ms = 3000;
const UTIL::Map JoystickTransmitter::s_time_to_uchar_map(s_max_time_ms,0,255,0);
const UTIL::Map JoystickTransmitter::s_value_to_char_map(32767,-32767,127,-127);

//----------------------------------------------------------------------//
JoystickTransmitter::JoystickTransmitter(const char* serial_port,
					 int baud,
					 const char* js_source) 
{
  d_js.reset(new JS::JoyStick(this,js_source));

  if (!d_js->init())
    {
      std::cout << "Joystick init failed" << std::endl;
      d_stay_running = false;
      return;
    }

  d_desc = serialOpen(serial_port, baud);
  if (d_desc == -1)
    {
      std::cout << "Could not open serial port" << std::endl;
      d_stay_running = false;
      return;
    }
  
  d_js->run();
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
void JoystickTransmitter::handleEvent(const js_event &event) 
{
//struct js_event {
  //	__u32 time;	/* event timestamp in milliseconds */
  //	__s16 value;	/* value */
  ///	__u8 type;	/* event type */
  //	__u8 number;	/* axis/button number */
  //};

  d_serial_cmd = "J";
  d_serial_cmd += convertValueTo8Bits(event.time, s_u32_max_digits);
  d_serial_cmd += convertValueTo8Bits(event.value, s_s16_max_digits);
  d_serial_cmd += convertValueTo8Bits(event.type, s_u8_max_digits);
  d_serial_cmd += convertValueTo8Bits(event.number, s_u8_max_digits);
  
  std::cout<<"serial: " << d_serial_cmd << std::endl;

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
  d_stay_running = false;
  d_js->stop();
}
