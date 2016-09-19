#include <iostream>

#include "coms_serialjoystick.h"

//make sure that xboxdrv is running before running this

int main()
{
  int baud = 9600;

  COMS::JoystickTransmitter js_tran;
  
  if (!js_tran.init("/dev/ttyAMA0",
		    baud,
		    "/dev/input/js0"))
    {
      std::cout<<"failed to initialize"<<std::endl;
      return 0;
    }

  KERN::KernBasic k(&js_tran);
  k.run();
}
