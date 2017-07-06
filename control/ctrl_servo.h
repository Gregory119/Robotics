#ifndef CTRL_SERVO_H
#define CTRL_SERVO_H

#include "ctrl_rcstepvelocitymanager.h"
#include "utl_mapping.h"
#include "kn_timer.h"

#include <cstdint>
#include <memory>
#include <mutex>

//NB: none of the functions are mutexed

namespace CTRL
{
  class Servo : KERN::KernelTimerOwner
  {
    //interface for servo controllers
  public:
    Servo(int max_pos=getMax8bitPos(), int min_pos=getMin8bitPos()); // minimum and maximum positions within the 8bit range
    virtual ~Servo();
    Servo(const Servo&) = delete;
    Servo& operator=(const Servo&) = delete;
    
    virtual void moveToPos(int pos) = 0; //0-255

    void setVelocityParams(int min_time_for_max_velocity_ms,
			   int time_step_ms); // useful for ESCs, which use the position here as the velocity of the motor
    void enableVelocityIncrementer(bool state);
    
    void setUsTiming(unsigned min_pulse,
		     unsigned max_pulse); //use your own units to conform to. The default values are in microseconds.
    
    unsigned getPulseMinTimeUs() { return d_min_pulse; }
    unsigned getPulseMaxTimeUs() { return d_max_pulse; }
    unsigned getSetPulseUs(); 

    static int getMax8bitPos();
    static int getMin8bitPos();
    static int getMid8bitPos();
    static int getRange8bitPos();
    int getMidPos();
    bool isPosValid(int); // uses internal max and min for limits

  protected:
    virtual void updateMove() = 0;
    void moveToStartPos(int pos);
    void setSetPos(int pos);
    void setReqPos(int pos);
    int getSetPos() { return d_set_pos; } // these are internally mapped positions and must not be used externally (use with caution)
    int getReqPos() { return d_req_pos; }
    void setReqPosDirect(int pos);

  protected:
    std::mutex d_m; // for now for thread safety, but need to create shared pipe reader/writer
    
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
    unsigned d_set_pulse = 500; 
    
    int d_req_pos = 0; 
    int d_set_pos = 0; 

    int d_max_pos = 255;
    int d_min_pos = 0;

    UTIL::Map d_pos_to_pulse;
    UTIL::Map d_pos_to_vel_map;

    bool d_has_velocity_inc = false;
    std::unique_ptr<RCStepVelocityManager> d_velocity_man;
    std::unique_ptr<KERN::KernelTimer> d_vel_inc_timer;
  };
};

#endif
