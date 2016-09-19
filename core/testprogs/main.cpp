#include "core_servo.h"

int main()
{
  using namespace CORE;

  unsigned pin = 1;
  Servo d_steering(pin);

  d_steering.run();
  
  return 0;
}
