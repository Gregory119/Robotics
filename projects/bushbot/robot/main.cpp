#include "pins.h"
#include "robot.h"

#include <iostream>

int main()
{
  Robot::setup();
  Robot::Params params(PIN::steering,
		       PIN::motor);
  Robot r(params);

  std::string serial_port = "/dev/ttyAMA0";
  int baud=9600;

  if (!r.init(serial_port.c_str(),
	      baud))
    {
      std::cout << "failed to initialize robot" << std::endl;
      return 0;
    }

  KERN::KernBasic k(&r);
  k.run();
  
  return 0;
}
