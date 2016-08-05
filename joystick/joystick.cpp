#include "joystick.h"

#include "common.h"

#include <linux/joystick.h>

#include <fcntl.h> //open
#include <unistd.h> //close

//#include <iostream> //std::cout

//----------------------------------------------------------------------//
JoyStick::JoyStick(JoyStickOwner* o, const char *dev_name)
  : d_owner(o),
    d_dev_name(dev_name)
{}

//----------------------------------------------------------------------//
JoyStick::~JoyStick()
{
  if (d_open)
    {
      close(d_js_desc);
    }
}

//----------------------------------------------------------------------//
bool JoyStick::init()
{
  if (!d_open)
    {
      d_js_desc = open(d_dev_name,O_RDONLY); //including O_NONBLOCK in the flags will cause io operations to return without waiting (returns error if not available straight away). No blocking causes io operations to wait for a return.
      /*
	This function is a cancellation point in multi-threaded programs. This is a problem if the thread allocates some resources (like memory, file descriptors, semaphores or whatever) at the time open is called. If the thread gets canceled these resources stay allocated until the program ends. To avoid this calls to open should be protected using cancellation handlers. 
      */
    }

  if (d_js_desc == s_error) 
    { d_open=false; }
  else 
    { d_open=true; }
  
  return d_open;
}

//----------------------------------------------------------------------//
void JoyStick::run()
{
  //run thread function and pass pointer to this class
  //in thread call the owner callback if the owner is not null
  //handle thread in the desctructor of this class
}

//----------------------------------------------------------------------//
bool JoyStick::readEvent()
{
  bool ret = false;
  js_event event_details;
  if (read(d_js_desc,&event_details,sizeof(event_details)) != s_error)
    {
      ret = false;
      switch (event_details.type)
	{
	case JS_EVENT_BUTTON:
	  {
	    JS_CO::ButtonEvent event(event_details);
	    d_owner->handleButton(event);
	  }
	  break;

	case JS_EVENT_AXIS:
	  {
	    JS_CO::AxisEvent event(event_details);
	    d_owner->handleAxis(event);
	  }
	  break;

	case JS_EVENT_INIT:
	  //inital state events
	  break;

	default:
	  //should not get this
	  break;
	}
    }
  else
    { ret = true; }

  return ret;
}

//----------------------------------------------------------------------//
//JoyStick::
