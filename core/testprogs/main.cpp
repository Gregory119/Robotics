#include "core_servo.h"

int main(int argc, char* argv[])
{
  using namespace CORE;

  unsigned pin = 1;
  Servo d_steering(pin);

  d_steering.run();
  d_steering.setPos(argv[1]);

  while(1){}
  
  return 0;
}
