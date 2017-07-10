//#include <iostream>
//#include <unistd.h> //for sleep

#include "djs_serialjoystick.h"
#include "kn_stdkernel.h"

#include <iostream>

int main()
{
  KERN::StdKernel k;
  int baud = 9600;

  D_JS::JoystickTransmitter js_tran;
  
  if (js_tran.init("/dev/ttyAMA0",
		   baud,
		   "/dev/input/js0"))
    {
      std::cout << "failed to initialize" << std::endl;
      return 0;
    }
  
  return k.run();
}
