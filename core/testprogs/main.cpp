#include "core_servo.h"

int main(int argc, char* argv[])
{
  using namespace CORE;

  Servo::setup();
  unsigned pin = 1;
  Servo d_steering(pin);

  d_steering.run();

  //'0' => 0, '255' => 255
  unsigned char pos = 0;
  pos = atoi(argv[1]);
  d_steering.setPos(pos); 
  
  while(1){}
  
  return 0;
}
