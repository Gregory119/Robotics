#include "ctrl_servo.h"

#include <cassert>
#include <iostream>

using namespace CTRL;

static const int s_max_8bit = 255;
static const int s_min_8bit = 0;

//----------------------------------------------------------------------//
Servo::Servo()
  : d_pos_to_pulse(UTIL::Map(s_max_8bit, s_min_8bit, d_max_pulse, d_min_pulse)),
    d_pos_to_vel_map(UTIL::Map(s_max_8bit,
			       s_min_8bit,
			       getMidPos(),
			       0-getMidPos())),
    d_vel_inc_timer(new KERN::KernelTimer(this))
{}

//----------------------------------------------------------------------//
Servo::~Servo() = default;

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
  if ((pos <= s_max_8bit) &&
      (pos >= s_min_8bit))
    {
      return true;
    }
  else
    {
      return false;
    }
}

//----------------------------------------------------------------------//
void Servo::setVelocityParams
(const RCStepVelocityManager::VelocityLimitParams& p)
{
  std::lock_guard<std::mutex> lock(d_m);
  assert(d_velocity_man == nullptr); //should only be done straight after initialisation.  This check is helpful for threading.

  d_velocity_man.reset(new RCStepVelocityManager(p));

  d_vel_inc_timer->setTimeMs(p.time_step_ms);
}

//----------------------------------------------------------------------//
void Servo::updateIncPos()
{
  std::lock_guard<std::mutex> lock(d_m);
  int req_vel = 0;
  int set_pos = 0;
  
  d_pos_to_vel_map.map(getReqPos(), req_vel); // pos used as velocity for ESCs
  int set_vel = d_velocity_man->stepVelocity(req_vel);
  d_pos_to_vel_map.inverseMap(set_vel, set_pos);
  setSetPos(set_pos);

  std::cout << "getReqPos() = " << (int)getReqPos() << std::endl;
  std::cout << "set_pos = " << (int)set_pos << std::endl;
}

//----------------------------------------------------------------------//
bool Servo::handleTimeOut(const KERN::KernelTimer& timer)
{
  std::lock_guard<std::mutex> lock(d_m);
  assert(d_has_velocity_inc);
  
  if (timer == d_vel_inc_timer.get())
    {
      std::cout << "timer timeout count: " << d_vel_inc_timer->getTotalTimeOuts() << std::endl;
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
  std::lock_guard<std::mutex> lock(d_m);
  assert(isPosValid(pos));
  
  if (!d_has_velocity_inc)
    {
      setSetPos(pos);
    }
  else
    {
      assert(d_velocity_man != nullptr);
      setReqPosDirect(pos);
      updateIncPos();
    }
}

//----------------------------------------------------------------------//
void Servo::moveToStartPos(int pos)
{
  std::lock_guard<std::mutex> lock(d_m);
  setReqPosDirect(pos);
  setSetPos(pos);
}

//----------------------------------------------------------------------//
void Servo::setSetPos(int pos)
{
  std::lock_guard<std::mutex> lock(d_m);
  assert(isPosValid(pos));
  d_set_pos = pos;
  d_pos_to_pulse.map(d_set_pos, d_set_pulse);
}

//----------------------------------------------------------------------//
void Servo::setReqPosDirect(int pos)
{
  std::lock_guard<std::mutex> lock(d_m);
  assert(isPosValid(pos));
  d_req_pos = pos;
}

//----------------------------------------------------------------------//
unsigned Servo::getSetPulseUs()
{
  std::lock_guard<std::mutex> lock(d_m);
  return d_set_pulse;
}

//----------------------------------------------------------------------//
int Servo::getMaxPos()
{
  return s_max_8bit;
}

//----------------------------------------------------------------------//
int Servo::getMinPos()
{
  return s_min_8bit;
}

//----------------------------------------------------------------------//
int Servo::getRangePos()
{
  return s_max_8bit - s_min_8bit;
}

//----------------------------------------------------------------------//
int Servo::getMidPos()
{
  return (s_max_8bit + s_min_8bit)/2;
}
