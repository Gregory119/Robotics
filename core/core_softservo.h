#ifndef CORE_SOFTSERVO_H
#define CORE_SOFTSERVO_H

#include "core_servo.h"

#include <future>

namespace CORE
{
  //SoftServo is a software based servo controller (not as accurate as hardware based, which usually results in servo arm jitters)
  //this class assumes a minimum pulse time of 1ms, maximum pulse time of 2ms, and a delay time of 20ms
  class SoftServo final : public Servo
  {
  public:
    explicit SoftServo(unsigned control_pin);
    ~SoftServo() {}
    SoftServo(const SoftServo&) = delete;
    SoftServo& operator=(const SoftServo&) = delete;

    void run();
    void stop();
   
    //The function wiringPiSetup() from libwiringPi in wiringPi.h must be called only once before using instances of this class. 
    //The function setup() calls wiringPiSetup() if you would prefer to call it that way.
    static void setup();

    void setDelayTimeUs(unsigned delay_us);    
    unsigned getDelayTimeUs();

    virtual void moveToPos(uint8_t pos) override; //0 < pos < 255

    virtual void setPosValue(uint8_t pos) override; //does not move the servo. It only sets the value
    virtual void setTiming(unsigned min_pulse,
			   unsigned max_pulse) override; //use your own units to conform to. The default values are in microseconds.
    
    virtual unsigned getPos() const override;
    virtual unsigned getPulseMinTimeUs() const override;
    virtual unsigned getPulseMaxTimeUs() const override;
    virtual const UTIL::Map& getPosMap() const override;

  private:
    void initPins();
    virtual void updatePos();
    static void threadFunc(std::future<bool> shutdown,
			   Servo *const servo);

  private:
    unsigned d_pin;
    unsigned d_delay_us = 20000;

    bool d_running = false;
    std::promise<bool> d_thread_shutdown;
    std::mutex d_m;
  };
};

#endif
