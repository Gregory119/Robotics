//extern
//{
#include "fcntl.h" //open
#include "unistd.h" //close
#include <linux/joystick.h>
#include <stdint.h>
#include <stdio.h>
  //#include "filter.h"
  //}

#include <iostream>
#include <cstdlib> //exit success/failure

int main(int argc, char *argv[])
{
  
  //int open (const char *filename, int flags[, mode_t mode])
  const char *js_source = "/dev/input/js0";
  int js_desc = open(js_source,O_RDONLY); //including O_NONBLOCK in the flags will cause io operations to return without waiting will likely errors. No blocking causing io commands to wait for a return.

  if (js_desc== -1)
    {
      std::cout << "Joystick did not open." << std::endl;
      return EXIT_FAILURE;
    }
  else
    {
      std::cout << "Joystick opened." << std::endl;
    }
  /*
  js_event {
	__u32 time;	// event timestamp in milliseconds 
	__s16 value;	// value 
	__u8 type;	// event type 
	__u8 number;	// axis/button number 
};
*/
  js_event event_details;

  while (read(js_desc,&event_details,sizeof(event_details))!=-1)
    {

      switch (event_details.type)
	{
	case JS_EVENT_BUTTON:
	  std::cout << "Button number: " << (int)event_details.number << ", value: " << event_details.value << std::endl;
	  break;

	case JS_EVENT_AXIS:
	  std::cout << "Axis number: " << (int)event_details.number << ", value: " << event_details.value << std::endl;
	  break;

	default:
	  break;
	}
    }

  std::cout << "Closing Joystick." << std::endl;
  close(js_desc);

  return EXIT_SUCCESS;
}
