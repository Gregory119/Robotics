#include "joystick.h"

#include "common.h"

#include <linux/joystick.h>

#include <fcntl.h> //open
#include <unistd.h> //close

JS_CO::Map s_stick_axis_map(32767,-32767,1.0,-1.0);
JS_CO::Map s_lever_axis_map(32767,-32767,0.0,1.0);

//----------------------------------------------------------------------//
JoyStick::JoyStick(const char *dev_name)
  : d_dev_name(dev_name)
{}

//----------------------------------------------------------------------//
bool JoyStick::open()
{
  d_js_desc = open(d_dev_name,O_RDONLY); //including O_NONBLOCK in the flags will cause io operations to return without waiting (returns error if not available straight away). No blocking causes io operations to wait for a return.

  if (d_js_desc == -1) 
    { return false; }
  else 
    { return true; }
}

//----------------------------------------------------------------------//
void JoyStick::run()
{
  //run thread function and pass pointer to this class
  //in thread call the owner callback if the owner is not null
  //handle thread in the desctructor of this class
}

//----------------------------------------------------------------------//
//JoyStick::
