#include "coms_serialjoystick.h"
#include <wiringSerial.h>
#include <cmath>

using namespace COMS;

static const uchar s_digits_u32 = 10;
static const uchar s_digits_s16 = 5;
static const uchar s_digits_u8 = 3;

//----------------------------------------------------------------------//
void JoystickTransmitter::JoystickTransmitter(const char* serial_port,
					      int baud,
					      const char* js_source) 
{
  d_js.reset(new JS::JoyStick(this,js_source););
  d_desc = serialOpen(source, baud);

  if ((!d_js.init()) || (d_desc == -1))
    {
      d_stay_running = false;
      return;
    }
  
  d_js.run();
}

//----------------------------------------------------------------------//
void JoystickTransmitter::~JoystickTransmitter()
{
  if (d_desc != -1)
    {
      serialClose(d_desc);
    }
}

//----------------------------------------------------------------------//
std::string convertValueToString(double value, unsigned max_digits)
{
  std::string ret;
  
  if (value < 0)
    {
      ret += '-';
    }

  for (unsigned i=0; i<max_digits; ++i)
    {
      ret += static_cast<char>(abs(value)/pow(10,max_digits-i-1)) + '0';
    }
}

//----------------------------------------------------------------------//
void JoystickTransmitter::handleEvent(const js_event &event) 
{
  d_serial_cmd = "JSE";
  d_serial_cmd += ":" + convertValueToString(event.time, s_digits_u32);
  d_serial_cmd += ":" + convertValueToString(event.value, s_digits_s16);
  d_serial_cmd += ":" + convertValueToString(event.type, s_digits_u8);
  d_serial_cmd += ":" + convertValueToString(event.number, s_digits_u8);
  d_serial_cmd += "#";
  
  serialPuts(d_desc, d_serial_cmd.c_str());
}

//struct js_event {
  //	__u32 time;	/* event timestamp in milliseconds */
  //	__s16 value;	/* value */
  ///	__u8 type;	/* event type */
  //	__u8 number;	/* axis/button number */
  //};

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
