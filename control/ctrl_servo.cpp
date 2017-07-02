#include "ctrl_servo.h"
#include "ctrl_rcstepvelocitymanager.h"

#include <cassert>

using namespace CTRL;

static const unsigned s_max_8bit = 255;
static const unsigned s_min_8bit = 0;

//----------------------------------------------------------------------//
Servo::Servo()
  : d_pos_8bit_to_pulse(UTIL::Map(getMaxPos(), getMinPos(), d_max_pulse, d_min_pulse)),
    d_servo_to_vel_map(UTIL::Map(getMaxPos(),
				 getMinPos(),
				 getMaxPos()-getMidPos(),
				 getMidPos()-getMaxPos())),
    d_vel_inc_timer(new KERN::KernelTimer(this))
{}

//----------------------------------------------------------------------//
Servo::~Servo() = default;

//----------------------------------------------------------------------//
void Servo::setUsTiming(unsigned min_pulse,
			unsigned max_pulse)
{
  d_min_pulse = min_pulse;
  d_max_pulse = max_pulse;
  d_pos_8bit_to_pulse = UTIL::Map(s_max_8bit, s_min_8bit, d_max_pulse, d_min_pulse);
}

//----------------------------------------------------------------------//
bool Servo::isPosInRange(uint8_t pos)
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
uint8_t Servo::getRangePos()
{
  return s_max_8bit - s_min_8bit;
}

//----------------------------------------------------------------------//
uint8_t Servo::getMaxPos()
{
  return s_max_8bit;
}

//----------------------------------------------------------------------//
uint8_t Servo::getMinPos()
{
  return s_min_8bit;
}

//----------------------------------------------------------------------//
uint8_t Servo::getMidPos()
{
  return (s_max_8bit + s_min_8bit)/2;
}

//----------------------------------------------------------------------//
void Servo::setVelocityParams(const VelocityParams& params)
{
  assert(d_velocity_man == nullptr); //should only be done straight after initialisation.  This check is helpful for threading.

  RCStepVelocityManager::VelocityLimitParams p;
  p.abs_max_velocity = d_servo_to_vel_map.d_out_max; //defined by the servo
  p.min_time_for_max_velocity_ms = params.min_time_for_max_velocity_ms;
  p.time_step_ms = params.time_step_ms;
  
  d_velocity_man.reset(new RCStepVelocityManager(p));

  d_vel_inc_timer->setTimeMs(params.time_step_ms);
}

//----------------------------------------------------------------------//
void Servo::updateIncPos()
{
  uint8_t req_vel = 0;
  mapFromTo(d_servo_to_vel_map, getReqPos(), req_vel);  // pos used as velocity for ESCs
  uint8_t set_vel = d_velocity_man->stepVelocity(req_vel);
  uint8_t set_pos = 0;
  mapFromTo(d_servo_to_vel_map, set_vel, set_pos, true);  // invert map
  setSetPos(set_pos);
}

//----------------------------------------------------------------------//
bool Servo::handleTimeOut(const KERN::KernelTimer& timer)
{
  assert(d_has_velocity_inc);
  
  if (timer == d_vel_inc_timer.get())
    {
      updateIncPos();
      updateMove();
      return true;
    }
  
  return false;
}

//----------------------------------------------------------------------//
void Servo::enableVelocityIncrementer(bool state)
{
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
void Servo::setReqPos(uint8_t pos)
{
  if (!d_has_velocity_inc)
    {
      setSetPos(pos);
    }
  else
    {
      assert(d_velocity_man != nullptr);
      d_req_pos = pos;
      updateIncPos();
    }
}
