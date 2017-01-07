#ifndef CORE_SOFTSERVO_H
#define CORE_SOFTSERVO_H

//#include "utl_mapping.h"

#include "core_servo.h"

//#include <cstdint>
//#include <future>

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
    void setDelayTimeUs(unsigned delay_us);    
    void run();
    void stop();
    
    virtual void moveToPos(uint8_t pos) override; //0 < pos < 255
    virtual bool incrementMove(uint8_t pos) override; //0 < pos < 255. Returns 1 with no problems. If an attempt to increment past a limit is made, that limiting position will be set and 0 will be returned.
    virtual bool decrementMove(uint8_t pos) override;
   
  private:
    void initPins();
    void updatePos();
    static void threadFunc(std::future<bool> shutdown,
			   SoftServo *const servo);

  private:
    unsigned d_pin;
    unsigned d_delay_us = 20000;

    bool d_running = false;
    std::promise<bool> d_thread_shutdown;
    std::mutex d_m;
  };
};

#endif
