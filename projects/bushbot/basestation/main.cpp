#include "djs_serialjoystick.h"
#include "kn_stdkernel.h"

#include <iostream>

int main()
{
  KERN::StdKernel k;
  
  int baud = 9600;
  std::string serial = "/dev/ttyAMA0";
  std::string joystick = "/dev/input/js0";

  D_JS::JoystickTransmitter js_tran;
  js_tran.setResendEventTimeoutMs(200);
  if (!js_tran.init(serial,
		    baud,
		    joystick))
    {
      std::cout<<"failed to initialize"<<std::endl;
      return 0;
    }
  js_tran.start();

  return k.run();
}
