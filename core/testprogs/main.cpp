#include "core_servo.h"

int main(int argc, char* argv[])
{
  using namespace CORE;

  Servo::setup();
  unsigned pin = 1;
  Servo d_steering(pin);

  d_steering.run();

  //'0' => 0, '1' => 255
  if (*argv[1] == '0')
    d_steering.setPos(0); 
  
  if (*argv[1] == '1')
    d_steering.setPos(255);
  
  while(1){}
  
  return 0;
}
