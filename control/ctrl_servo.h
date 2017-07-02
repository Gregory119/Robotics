#ifndef CTRL_SERVO_H
#define CTRL_SERVO_H

#include "utl_mapping.h"
#include "kn_timer.h"

#include <cstdint>
#include <memory>

//NB: none of the functions are mutexed

namespace UTIL
{
  struct Map;
};

namespace CTRL
{
  //interface for servo controllers
  class RCStepVelocityManager;
  class Servo : KERN::KernelTimerOwner
  {
  public:
    struct VelocityParams
    {
      int min_time_for_max_velocity_ms = 1;
      int time_step_ms = 0;
    };
    
  public:
    Servo();
    virtual ~Servo();
    Servo(const Servo&) = delete;
    Servo& operator=(const Servo&) = delete;
    
    virtual void moveToPos(uint8_t pos) = 0;

    void setVelocityParams(const VelocityParams&); // useful for ESCs, which use the position here as the velocity of the motor
    void enableVelocityIncrementer(bool state);
    
    virtual void setUsTiming(unsigned min_pulse,
			     unsigned max_pulse); //use your own units to conform to. The default values are in microseconds.
    
    virtual unsigned getSetPos() { return d_set_pos; }
    virtual unsigned getPulseMinTimeUs() { return d_min_pulse; }
    virtual unsigned getPulseMaxTimeUs() { return d_max_pulse; }
    virtual const UTIL::Map& getPosMap() { return d_pos_8bit_to_pulse; }
    static uint8_t getRangePos();
    static uint8_t getMaxPos();
    static uint8_t getMinPos();
    static uint8_t getMidPos();
    static bool isPosInRange(uint8_t pos);

  protected:
    virtual void updateMove() = 0;
    virtual void moveToStartPos(uint8_t pos);
    virtual void setSetPos(uint8_t pos) { d_set_pos = pos; }
    virtual void setReqPos(uint8_t pos);
    virtual uint8_t getReqPos() { return d_req_pos; }
    virtual void setReqPosDirect(uint8_t pos) { d_req_pos = pos; }
    
  private:
    // KERN::KernelTimerOwner
    bool handleTimeOut(const KERN::KernelTimer& timer);
    
    void updateIncPos();

  private:
    /*
      hobby servos:
      d_min_pulse = 500;
      d_max_pulse = 2500;

      rc car steering servos:
      d_min_pulse = 1000;
      d_max_pulse = 2000;
    */        
    unsigned d_min_pulse = 500; //default values in us
    unsigned d_max_pulse = 2500;
    uint8_t d_req_pos = 0; //is in the 0-255 range
    uint8_t d_set_pos = 0; //is in the 0-255 range
    
    UTIL::Map d_pos_8bit_to_pulse;
    UTIL::Map d_servo_to_vel_map;
    UTIL::Map d_vel_to_servo_map;

    bool d_has_velocity_inc = false;
    std::unique_ptr<RCStepVelocityManager> d_velocity_man;
    std::unique_ptr<KERN::KernelTimer> d_vel_inc_timer;
  };
};

#endif
