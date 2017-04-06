#include "djs_serialjoystick.h"

#include <iostream>

int main()
{
  int baud = 9600;

  D_JS::JoystickReceiver js_receiver;
  D_JS::JSEventMinimal js_event;
  
  if (js_receiver.init("/dev/ttyAMA0",
		       baud))
    {
      while (1)
	{
	  if(js_receiver.readSerialEvent(js_event))
	    {
	      std::cout << "time [ms]: " << (int)js_event.time_ms << std::endl;
	      std::cout << "value: " << (int)js_event.value << std::endl;
	      std::cout << "type: " << (int)js_event.type << std::endl;
	      std::cout << "number: " << (int)js_event.number << std::endl;
	    }
	}
    }
}
