#ifndef CTRL_SOFTSERVO_H
#define CTRL_SOFTSERVO_H

#include "ctrl_servo.h"

#include <future>

namespace CTRL
{
  //SoftServo is a software based servo controller (not as accurate as hardware based, thus it usually results in servo arm jitters)
  //this class assumes a minimum pulse time of 1ms, maximum pulse time of 2ms, and a delay time of 20ms
  class SoftServo final : public Servo
  {
  public:
    explicit SoftServo(unsigned control_pin,
		       int max_pos=getMax8bitPos(),
		       int min_pos=getMin8bitPos());
    ~SoftServo() { stop(); }
    SoftServo(const SoftServo&) = delete;
    SoftServo& operator=(const SoftServo&) = delete;

    void run();
    void stop();
   
    //The function wiringPiSetup() from libwiringPi in wiringPi.h must be called only once before using instances of this class. 
    //The function setup() calls wiringPiSetup() if you would prefer to call it that way.
    static void setup();

    void moveToPos(int pos) override; //0 < pos < 255

  private:
    //Servo
    void updateMove() override;
    
    static void threadFunc(std::future<bool> shutdown,
			   SoftServo *const servo);
    unsigned getDelayTimeUs();

  private:
    unsigned d_pin;
    unsigned d_delay_us = 20000;

    bool d_running = false;
    std::promise<bool> d_thread_shutdown;

    bool d_first_move = true;
  };
};

#endif
