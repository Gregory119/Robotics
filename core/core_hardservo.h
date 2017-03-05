#ifndef CORE_HARDERVO_H
#define CORE_HARDSERVO_H

#include "core_servo.h"

namespace CORE
{
  //HardServo is a hardware based servo controller, which requires servoblaster installed and running in the background
  class HardServo final : public Servo
  {
  public:
    //see https://github.com/richardghirst/PiBits/tree/master/ServoBlaster for available servo numbers and their respective pin connections
    explicit HardServo(unsigned servo_num);
    ~HardServo() {}
    HardServo(const HardServo&) = delete;
    HardServo& operator=(const HardServo&) = delete; //uses the same settings
    
    virtual void moveToPos(uint8_t pos) override; //0 < pos < 255
    
  private:
    unsigned d_servo_num = 0;
  };
};

#endif
