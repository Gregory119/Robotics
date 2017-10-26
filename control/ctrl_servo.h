#ifndef CTRL_SERVO_H
#define CTRL_SERVO_H

#include "ctrl_rcstepvelocitymanager.h"
#include "utl_mapping.h"
#include "kn_asiocallbacktimer.h"

#include <cstdint>
#include <memory>
#include <mutex>

//NB: none of the functions are mutexed

namespace CTRL
{
  class Servo
  {
    // Interface for servo controllers
    // All position values must be in the same units
  public:
    Servo(); // minimum and maximum positions within the 8bit range
    virtual ~Servo();
    Servo(const Servo&) = delete;
    Servo& operator=(const Servo&) = delete;
    
    virtual void moveToPos(int pos) = 0; // default: 0-2000, otherwise within set max and min input values
    void moveToMidPos() { moveToPos(d_mid_pos); }

    void setAsESC(); // uses general ESC pulse timings
    void setVelocityParams(int min_time_for_max_velocity_ms,
			   int time_step_ms); // useful for ESCs, which use the position here as the velocity of the motor

    void setOutputPosLimits(int min_pos, int max_pos); 
    void setOutputPosLimits(int min_pos, int max_pos, int mid_pos); // Map the input values to different actual output servo positions.
    void setInputPosLimits(int min_pos, int max_pos);

    void setUsTiming(unsigned min_pulse,
		     unsigned max_pulse);
    
    unsigned getPulseMinTimeUs() { return d_min_pulse; }
    unsigned getPulseMaxTimeUs() { return d_max_pulse; }
    unsigned getSetPulseUs(); 

    int getMaxInputPos();
    int getMinInputPos();
    int getMidInputPos();
    int getRangeInputPos();

  protected:
    virtual void updateMove() = 0;
    void moveToStartPos(int pos);
    void setSetPos(int pos);
    void setReqPos(int pos);
    bool isPosValid(int);

  protected:
    std::mutex d_m; // for now for thread safety, but need to create shared pipe reader/writer
    
  private:
    void setReqPosDirect(int pos);
    void updateIncPos();

  private:
    /*
      hobby servos:
      d_min_pulse = 500;
      d_max_pulse = 2500;

      rc car steering (ESCs) servos:
      d_min_pulse = 1000;
      d_max_pulse = 2000;
    */        
    unsigned d_min_pulse = 500; //default values in us
    unsigned d_max_pulse = 2500;
    unsigned d_set_pulse = 500; 

    unsigned d_max_out_pulse = d_max_pulse;
    unsigned d_min_out_pulse = d_min_pulse;
    unsigned d_mid_out_pulse = (d_max_out_pulse+d_min_out_pulse)/2;
    
    int d_req_pos = 0; 
    int d_set_pos = 0; 

    int d_max_pos = 2000;
    int d_min_pos = 0;
    int d_mid_pos = (d_max_pos+d_min_pos)/2;
    
    UTIL::Map d_pos_to_pulse;
    UTIL::Map d_pos_to_vel_map;

    std::unique_ptr<RCStepVelocityManager> d_velocity_man;
    KERN::AsioCallbackTimer d_vel_inc_timer;
  };
};

#endif
