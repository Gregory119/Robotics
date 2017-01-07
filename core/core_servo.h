#ifndef CORE_SERVO_H
#define CORE_SERVO_H

#include "utl_mapping.h"

#include <cstdint>

namespace CORE
{
  //interface for servo controllers
  class Servo
  {
  public:
    Servo();
    virtual ~Servo();
    Servo(const Servo&); //uses the same settings
    Servo& operator=(const Servo&); //uses the same settings
    
    virtual void moveToPos(uint8_t pos) = 0; //0 < pos < 255
    virtual bool incrementMove(uint8_t pos) = 0; //0 < pos < 255. Returns 1 with no problems. If an attempt to increment past a limit is made, that limiting position will be set and 0 will be returned.
    virtual bool decrementMove(uint8_t pos) = 0;

    void setPosValue(uint8_t pos) { d_pos = pos; } //does not move the servo. It only sets the value
    void setTiming(unsigned min_pulse,
		   unsigned max_pulse); //use your own units to conform to. The default values are in microseconds.
    
    unsigned getPos() { return d_pos; }
    static uint8_t getMaxPos();
    static uint8_t getMinPos();
    unsigned getPulseMinTimeUs() { return d_min_pulse; }
    unsigned getPulseMaxTimeUs() { return d_max_pulse; }
    const UTIL::Map& getPosMap() { return d_pos_8bit_to_pulse; }
    bool isPosInRange(uint8_t pos);

  private:
    unsigned d_min_pulse = 500; //default values in us
    unsigned d_max_pulse = 2500;
    unsigned d_pos = 0; //is in the 0-255 range
        
    UTIL::Map d_pos_8bit_to_pulse;
  };
};

#endif
