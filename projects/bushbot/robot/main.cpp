#include "servonums.h"
#include "robot.h"

#include "kn_asiokernel.h"

#include <iostream>

int main()
{
  KERN::AsioKernel k;
  
  Robot::Params params(SERVONUM::steering,
		       SERVONUM::motor);
  Robot r(params);

  std::string serial_port = "/dev/ttyAMA0";
  int baud=9600;

  if (!r.init(serial_port,
	      baud))
    {
      std::cout << "failed to initialize robot" << std::endl;
      return 0;
    }
  
  k.run();
  return 0;
}
