#include "core_softservo.h"

#include <cstdlib>

#include <iostream>

int main(int argc, char* argv[])
{
  using namespace CORE;

  if (argc < 3)
    {
      std::cout << "parameters: [wiringPi pin] [position: 0-255]" << std::endl;
      return 0;
    }

  SoftServo::setup();
  unsigned pin = atoi(argv[1]);
  SoftServo d_steering(pin);
  d_steering.run();

  unsigned char pos = 0;
  pos = atoi(argv[2]);
  d_steering.moveToPos(pos); 
  
  while(1){}
  
  return 0;
}
