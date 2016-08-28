//#include <iostream>
//#include <unistd.h> //for sleep


#include "coms_serialjoystick.h"

int main()
{
  int baud = 9600;

  COMS::JoystickTransmitter js_tran("/dev/ttyAMA0",
				    baud,
				    "/dev/input/js0");

  KERN::KernBasic k(&js_tran);
  k.run();
}
