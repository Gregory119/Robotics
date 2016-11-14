#ifndef CORE_SERVO_H
#define CORE_SERVO_H

#include "utl_mapping.h"

#include <cstdint>
#include <future>

namespace CORE
{
  //this class assumes a minimum pulse time of 1ms, maximum pulse time of 2ms, and a delay time of 40ms
  class Servo final
  {
  public:
    //The function wiringPiSetup() from libwiringPi in wiringPi.h must be called only once before using instances of this class. 
    //The function setup() calls wiringPiSetup() if you would prefer to call it that way.
    explicit Servo(unsigned control_pin);
    ~Servo();
    Servo(const Servo&); //uses the same settings
    Servo& operator=(const Servo&); //uses the same settings
    
    static void setup();

    void setTiming(unsigned min_us,
		   unsigned max_us,
		   unsigned delay_us); //this must be optionally set before running
    
    void run();
    void stop();
    
    void setPos(uint8_t pos); //0 < pos < 255
    bool incrementPos(uint8_t pos); //0 < pos < 255. Returns 1 with no problems. If an attempt to increment past a limit is made, that limiting position will be set and 0 will be returned.
    bool decrementPos(uint8_t pos);
   
  public:
    static const unsigned char max_pos = 255;
    static const unsigned char min_pos = 0;
 
  private:
    void initPins();
    void updatePos();
    static void threadFunc(std::future<bool> shutdown,
			   Servo *const servo);

  private:
    unsigned d_pin;
    unsigned d_min_us = 500;
    unsigned d_max_us = 2500;
    unsigned d_delay_us = 40000;
    unsigned d_pos = 0;
    
    UTIL::Map d_pos_8bit_to_us;

    bool d_running = false;
    std::promise<bool> d_thread_shutdown;
    std::mutex d_m;
  };
};

#endif
