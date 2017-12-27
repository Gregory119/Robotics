#include "ctrl_servo.h"

#include <cassert>
#include <iostream>

using namespace CTRL;

//----------------------------------------------------------------------//
Servo::Servo()
  : d_pos_to_pulse(UTIL::Map(d_max_pos, d_min_pos, d_max_pulse, d_min_pulse)),
    d_pos_to_vel_map(UTIL::Map(d_max_pos,
			       d_min_pos,
			       d_max_pos - d_mid_pos,
			       d_mid_pos - d_max_pos))
{
  d_vel_inc_timer.setTimeoutCallback([&](){
      assert(d_velocity_man != nullptr);
      //std::cout << "timer timeout count: " << d_vel_inc_timer->getTotalTimeOuts() << std::endl;
      updateIncPos();
      updateMove();
    });
}

//----------------------------------------------------------------------//
Servo::~Servo() = default;

//----------------------------------------------------------------------//
void Servo::setUsTiming(unsigned min_pulse,
			unsigned max_pulse)
{
  assert(max_pulse > min_pulse);

  std::lock_guard<std::mutex> lock(d_m);
  d_min_pulse = min_pulse;
  d_max_pulse = max_pulse;
  d_max_out_pulse = d_max_pulse;
  d_min_out_pulse = d_min_pulse;
  d_mid_out_pulse = (d_max_out_pulse+d_min_out_pulse)/2;
    
  d_pos_to_pulse = UTIL::Map(d_max_pos,
			     d_min_pos,
			     d_max_pulse,
			     d_min_pulse);
}

//----------------------------------------------------------------------//
bool Servo::isPosValid(int pos)
{
  std::lock_guard<std::mutex> lock(d_m);
  if ((pos <= d_max_pos) &&
      (pos >= d_min_pos))
    {
      return true;
    }
  else
    {
      return false;
    }
}

//----------------------------------------------------------------------//
void Servo::setVelocityParams(int min_time_for_max_velocity_ms,
			      int time_step_ms)
{
  std::lock_guard<std::mutex> lock(d_m);
  assert(d_velocity_man == nullptr); //should only be done straight after initialisation.  This check is helpful for threading.

  RCStepVelocityManager::VelocityLimitParams p;
  p.abs_max_velocity = d_max_pos - d_mid_pos;
  p.min_time_for_max_velocity_ms = min_time_for_max_velocity_ms;
  p.time_step_ms = time_step_ms;
  
  d_velocity_man.reset(new RCStepVelocityManager(p));

  d_vel_inc_timer.restartMs(p.time_step_ms);
}

//----------------------------------------------------------------------//
void Servo::updateIncPos()
{
  assert(d_velocity_man != nullptr);
  int req_vel = 0;
  int set_pos = 0;
  
  d_pos_to_vel_map.map(d_req_pos, req_vel); // pos used as velocity for ESCs
  //std::cout << "d_req_pos = " << (int)d_req_pos << std::endl;
  //std::cout << "req_vel = " << (int)req_vel << std::endl;
  int set_vel = d_velocity_man->stepToVelocity(req_vel);
  //std::cout << "set_vel = " << (int)set_vel << std::endl;
  d_pos_to_vel_map.inverseMap(set_vel, set_pos);
  //std::cout << "set_pos = " << (int)set_pos << std::endl;
  //std::cout.flush();
  setSetPos(set_pos);
}

//----------------------------------------------------------------------//
void Servo::setReqPos(int pos)
{
  assert(isPosValid(pos));
  
  if (d_velocity_man == nullptr)
    {
      setSetPos(pos);
    }
  else
    {
      setReqPosDirect(pos);
    }
}

//----------------------------------------------------------------------//
void Servo::moveToStartPos(int pos)
{
  setReqPosDirect(pos);
  //std::cout << "moveToStartPos" << std::endl;
  //std::cout << "pos = " << pos << std::endl;
  setSetPos(pos);
}

//----------------------------------------------------------------------//
void Servo::setSetPos(int pos)
{
  assert(isPosValid(pos));
  std::lock_guard<std::mutex> lock(d_m);
  d_set_pos = pos;
  d_pos_to_pulse.map(d_set_pos, d_set_pulse);
}

//----------------------------------------------------------------------//
void Servo::setReqPosDirect(int pos)
{
  assert(isPosValid(pos));
  std::lock_guard<std::mutex> lock(d_m);
  d_req_pos = pos;
}

//----------------------------------------------------------------------//
unsigned Servo::getSetPulseUs()
{
  std::lock_guard<std::mutex> lock(d_m);
  return d_set_pulse;
}

//----------------------------------------------------------------------//
void Servo::setAsESC()
{
  setUsTiming(1000, 2000);
}

//----------------------------------------------------------------------//
int Servo::getMaxInputPos()
{
  std::lock_guard<std::mutex> lock(d_m);
  return d_max_pos;
}

//----------------------------------------------------------------------//
int Servo::getMinInputPos()
{
  std::lock_guard<std::mutex> lock(d_m);
  return d_min_pos;
}

//----------------------------------------------------------------------//
int Servo::getMidInputPos()
{
  std::lock_guard<std::mutex> lock(d_m);
  return d_mid_pos;
}

//----------------------------------------------------------------------//
int Servo::getRangeInputPos()
{
  std::lock_guard<std::mutex> lock(d_m);
  return d_max_pos - d_min_pos;
}

//----------------------------------------------------------------------//  
void Servo::setInputPosLimits(int min_pos, int max_pos)
{
  assert(max_pos > min_pos);
  std::lock_guard<std::mutex> lock(d_m);
  d_min_pos = min_pos;
  d_max_pos = max_pos;
  d_mid_pos = (d_max_pos+d_min_pos)/2;
  d_pos_to_pulse = UTIL::Map(d_max_pos,
			     d_min_pos,
			     d_max_out_pulse, // output pulses may have already been set when calling this function
			     d_min_out_pulse,
			     d_mid_out_pulse);
}

//----------------------------------------------------------------------//
void Servo::setOutputPosLimits(int min_pos, int max_pos, int mid_pos)
{
  std::unique_lock<std::mutex> lock_f(d_m);
  assert(min_pos > d_min_pos);
  assert(max_pos < d_max_pos);
  lock_f.unlock();
  assert(mid_pos > min_pos);
  assert(mid_pos < max_pos);

  std::lock_guard<std::mutex> lock_s(d_m);
  UTIL::Map pos_to_out_pulse(d_max_pos, d_min_pos, d_max_pulse, d_min_pulse);
  d_max_out_pulse = pos_to_out_pulse.map<int>(max_pos);
  d_min_out_pulse = pos_to_out_pulse.map<int>(min_pos);
  d_mid_out_pulse = pos_to_out_pulse.map<int>(mid_pos);
  assert(d_max_out_pulse <= d_max_pulse);
  assert(d_min_out_pulse >= d_min_pulse);
  assert(d_mid_out_pulse > d_min_pulse);
  assert(d_mid_out_pulse < d_max_pulse);
  
  d_pos_to_pulse = UTIL::Map(d_max_pos,
			     d_min_pos,
			     d_max_out_pulse,
			     d_min_out_pulse,
			     d_mid_out_pulse);
}

//----------------------------------------------------------------------//
void Servo::setOutputPosLimits(int min_pos, int max_pos)
{
  setOutputPosLimits(min_pos, max_pos, (max_pos+min_pos)/2);
}
