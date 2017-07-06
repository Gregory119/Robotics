#ifndef CTRL_HARDERVO_H
#define CTRL_HARDSERVO_H

#include "ctrl_servo.h"

namespace CTRL
{
  //HardServo is a hardware based servo controller, which requires servoblaster installed and running in the background
  class HardServo final : public Servo
  {
  public:
    //see https://github.com/richardghirst/PiBits/tree/master/ServoBlaster for available servo numbers and their respective pin connections
    explicit HardServo(unsigned servo_num,
		       int max_pos=getMax8bitPos(),
		       int min_pos=getMin8bitPos());
    ~HardServo() {}
    HardServo(const HardServo&) = delete;
    HardServo& operator=(const HardServo&) = delete; //uses the same settings
    
    void moveToPos(int pos) override; //0 < pos < 255

  private:
    void updateMove() override;
    
  private:
    unsigned d_servo_num = 0;
    bool d_first_move = true;
  };
};

#endif
