#include "core_hardservo.h"

#include <cstdlib>

int main(int argc, char* argv[])
{
  using namespace CORE;

  Servo::setup();
  unsigned servo_num = 2;
  HardServo d_steering(servo_num);

  //'0' => 0, '255' => 255
  unsigned char pos = 0;
  pos = atoi(argv[1]);
  d_steering.moveToPos(pos); 
  
  while(1){}
  
  return 0;
}
