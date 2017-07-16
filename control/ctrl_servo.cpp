#include "ctrl_servo.h"

#include <cassert>
#include <iostream>

using namespace CTRL;

static const int s_max_8bit = 255;
static const int s_min_8bit = 0;

//----------------------------------------------------------------------//
Servo::Servo(int max_pos, int min_pos)
  : d_max_pos(max_pos),
    d_min_pos(min_pos),
    d_pos_to_pulse(UTIL::Map(s_max_8bit, s_min_8bit, d_max_pulse, d_min_pulse)),
    d_pos_to_vel_map(UTIL::Map(d_max_pos,
			       d_min_pos,
			       d_max_pos - getMidPos(),
			       getMidPos() - d_max_pos)),
    d_vel_inc_timer(new KERN::KernelTimer(this))
{
  assert(max_pos <= s_max_8bit);
  assert(min_pos >= s_min_8bit);
}

//----------------------------------------------------------------------//
Servo::~Servo() = default;

//----------------------------------------------------------------------//
int Servo::getMidPos()
{
  return (d_max_pos+d_min_pos)/2;
}

//----------------------------------------------------------------------//
void Servo::setUsTiming(unsigned min_pulse,
			unsigned max_pulse)
{
  std::lock_guard<std::mutex> lock(d_m);
  d_min_pulse = min_pulse;
  d_max_pulse = max_pulse;
  d_pos_to_pulse = UTIL::Map(s_max_8bit, s_min_8bit, d_max_pulse, d_min_pulse);
}

//----------------------------------------------------------------------//
bool Servo::isPosValid(int pos)
{
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
  p.abs_max_velocity = d_max_pos - getMidPos();
  p.min_time_for_max_velocity_ms = min_time_for_max_velocity_ms;
  p.time_step_ms = time_step_ms;
  
  d_velocity_man.reset(new RCStepVelocityManager(p));

  d_vel_inc_timer->setTimeMs(p.time_step_ms);
}

//----------------------------------------------------------------------//
void Servo::updateIncPos()
{  
  int req_vel = 0;
  int set_pos = 0;
  
  d_pos_to_vel_map.map(getReqPos(), req_vel); // pos used as velocity for ESCs
  //std::cout << "getReqPos() = " << (int)getReqPos() << std::endl;
  //std::cout << "req_vel = " << (int)req_vel << std::endl;
  int set_vel = d_velocity_man->stepToVelocity(req_vel);
  //std::cout << "set_vel = " << (int)set_vel << std::endl;
  d_pos_to_vel_map.inverseMap(set_vel, set_pos);
  //std::cout << "set_pos = " << (int)set_pos << std::endl;
  //std::cout.flush();
  setSetPos(set_pos);
}

//----------------------------------------------------------------------//
bool Servo::handleTimeOut(const KERN::KernelTimer& timer)
{
  assert(d_has_velocity_inc);
  
  if (timer == d_vel_inc_timer.get())
    {
      //std::cout << "timer timeout count: " << d_vel_inc_timer->getTotalTimeOuts() << std::endl;
      updateIncPos();
      updateMove();
      return true;
    }
  
  return false;
}

//----------------------------------------------------------------------//
void Servo::enableVelocityIncrementer(bool state)
{
  std::lock_guard<std::mutex> lock(d_m);
  assert(d_velocity_man != nullptr); //should only be done straight after initialisation.  This check is helpful for threading.
  assert(d_vel_inc_timer != nullptr);
  
  d_has_velocity_inc = state;
  if (state)
    {
      d_vel_inc_timer->restart();
    }
  else
    {
      d_vel_inc_timer->disable();
    }
} 

//----------------------------------------------------------------------//
void Servo::setReqPos(int pos)
{
  assert(isPosValid(pos));
  
  if (!d_has_velocity_inc)
    {
      setSetPos(pos);
    }
  else
    {
      assert(d_velocity_man != nullptr);
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
int Servo::getMax8bitPos()
{
  return s_max_8bit;
}

//----------------------------------------------------------------------//
int Servo::getMin8bitPos()
{
  return s_min_8bit;
}

//----------------------------------------------------------------------//
int Servo::getRange8bitPos()
{
  return s_max_8bit - s_min_8bit;
}

//----------------------------------------------------------------------//
int Servo::getMid8bitPos()
{
  return (s_max_8bit + s_min_8bit)/2;
}
