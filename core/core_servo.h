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
    Servo(unsigned control_pin);
    ~Servo();
    Servo(const Servo&)=delete;
    Servo& operator=(const Servo&)=delete;
    
    void setTiming(unsigned min_us,
		   unsigned max_us,
		   unsigned delay_us); //this must be optionally set before running
    
    void run();
    void stop();
    
    void setPos(uint8_t pos); //0 < pos < 255
    bool incrementPos(uint8_t pos); //0 < pos < 255. Returns 1 with no problems. If an attempt to increment past a limit is made, that limiting position will be set and 0 will be returned.
    bool decrementPos(uint8_t pos);
    
  private:
    typedef void (*callbackFunction)(void);
    void updatePos();
    static void threadFunc(std::future<bool> shutdown,
			   Servo *const servo);

  private:
    unsigned d_pin;
    unsigned d_min_us = 1000;
    unsigned d_max_us = 2000;
    unsigned d_delay_us = 40000;
    unsigned d_pos = 0;

UTIL::Map d_pos_8bit_to_us;

    bool d_running = false;
    std::promise<bool> d_thread_shutdown;
    std::mutex m;
  };
};

#endif
