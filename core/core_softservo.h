#ifndef CORE_SOFTSERVO_H
#define CORE_SOFTSERVO_H

#include "core_servo.h"

namespace CORE
{
  //SoftServo is a software based servo controller (not as accurate as hardware based, which usually results in servo arm jitters)
  //this class assumes a minimum pulse time of 1ms, maximum pulse time of 2ms, and a delay time of 20ms
  class SoftServo final : public Servo
  {
  public:
    explicit SoftServo(unsigned control_pin);
    ~SoftServo();
    SoftServo(const SoftServo&); //uses the same settings
    SoftServo& operator=(const SoftServo&); //uses the same settings
    
    //The function wiringPiSetup() from libwiringPi in wiringPi.h must be called only once before using instances of this class. 
    //The function setup() calls wiringPiSetup() if you would prefer to call it that way.
    static void setup();
   
  private:
    void initPins();
    virtual void updatePos() override;

  private:
    unsigned d_pin;
  };
};

#endif
