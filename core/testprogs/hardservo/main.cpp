#include "core_hardservo.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
  using namespace CORE;

  if (argc < 3)
    {
      std::cout << "parameters: [servo number] [position: 0-255]" << std::endl;
    }

  unsigned servo_num = atoi(argv[1]);;
  if (servo_num < 0 ||
      servo_num > 7)
    {
      std::cout << "servo number out of range" << std::endl;
      return 0;
    }
  HardServo d_steering(servo_num);

  unsigned char pos = 0;
  pos = atoi(argv[2]);
  d_steering.moveToPos(pos); 
  
  return 0;
}
