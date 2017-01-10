#include "core_softservo.h"

#include <cstdlib>

int main(int argc, char* argv[])
{
  using namespace CORE;

  Servo::setup();
  unsigned pin = 1;
  SoftServo d_steering(pin);

  //'0' => 0, '255' => 255
  unsigned char pos = 0;
  pos = atoi(argv[1]);
  d_steering.moveToPos(pos); 
  
  while(1){}
  
  return 0;
}
