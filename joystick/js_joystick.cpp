#include "js_joystick.h"

#include "js_common.h"

#include <fcntl.h> //open
#include <unistd.h> //close
#include <thread>

#include <iostream> //std::cout

using namespace JS;

//----------------------------------------------------------------------//
JoyStick::JoyStick(JoyStickOwner* o, const char *dev_name)
  : d_owner(o),
    d_dev_name(dev_name)
{}

//----------------------------------------------------------------------//
JoyStick::~JoyStick()
{
  std::cout << "~JoyStick()" << std::endl;
  if (d_running) { stop(); }
  if (d_open) { close(d_js_desc); }
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

  if (d_js_desc == s_error) { d_open=false; }
  else { d_open=true; }
  
  return d_open;
}

//----------------------------------------------------------------------//
void JoyStick::run()
{
  d_running = true;
  std::thread t(&JoyStick::threadFunc,
		d_thread_shutdown.get_future(),
		this);
  t.detach();
}

//----------------------------------------------------------------------//
void JoyStick::stop()
{
  d_running = false;
  d_thread_shutdown.set_value(true);
}

//----------------------------------------------------------------------//
void JoyStick::threadFunc(std::future<bool> shutdown,
			  const JoyStick* js)
{
  while (shutdown.wait_for(std::chrono::nanoseconds(0)) != 
	 std::future_status::ready)
    {
      if (!js->readEvent())
	{
	  js->d_owner->handleReadError();
	}
    }
}

//----------------------------------------------------------------------//
bool JoyStick::readEvent() const
{
  bool ret = false;
  js_event event_details;
  if (read(d_js_desc,&event_details,sizeof(event_details)) != s_error)
    {
      ret = true;
      d_owner->handleEvent(event_details);
    }

  return ret;
}

//----------------------------------------------------------------------//
//JoyStick::
