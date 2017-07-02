#include "servonums.h"
#include "robot.h"

#include "kn_stdkernel.h"

#include <iostream>

int main()
{
  KERN::StdKernel k;
  
  Robot::Params params(SERVONUM::steering,
		       SERVONUM::motor);
  Robot r(params);

  std::string serial_port = "/dev/ttyAMA0";
  int baud=9600;

  if (!r.init(serial_port.c_str(),
	      baud))
    {
      std::cout << "failed to initialize robot" << std::endl;
      return 0;
    }
  
  return k.run();
}
