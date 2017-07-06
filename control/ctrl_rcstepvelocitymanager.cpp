#include "ctrl_rcstepvelocitymanager.h"
#include "core_stepincrementer.h"

#include "utl_values.h"

#include <cassert>
#include <iostream>

using namespace CTRL;

//----------------------------------------------------------------------//
// RCStepVelocityManager
//----------------------------------------------------------------------//
RCStepVelocityManager::RCStepVelocityManager(const VelocityLimitParams& p)
  : d_vel_inc(new CORE::StepIncrementer(static_cast<double>(p.abs_max_velocity)/p.min_time_for_max_velocity_ms,
					 p.time_step_ms))
{
  // add list of possible states
  d_states[VelocityStateId::Pos] = std::unique_ptr<VelocityState>(new PosVelocityState());
  d_states[VelocityStateId::Neg] = std::unique_ptr<VelocityState>(new NegVelocityState());
  d_states[VelocityStateId::Zero] = std::unique_ptr<VelocityState>(new ZeroVelocityState());
  d_state = d_states[VelocityStateId::Zero].get();
}

//----------------------------------------------------------------------//
RCStepVelocityManager::~RCStepVelocityManager() = default;

//----------------------------------------------------------------------//
void RCStepVelocityManager::updateState()
{
  d_state = d_states[d_next_state_id].get();
  assert(d_state!=nullptr);
}

//----------------------------------------------------------------------//
void RCStepVelocityManager::setNextState(VelocityStateId id)
{
  d_next_state_id = id;
}

//----------------------------------------------------------------------//
int RCStepVelocityManager::stepToVelocity(int vel)
{
  assert(d_state != nullptr);
  assert(d_vel_inc != nullptr);

  int vel_diff = 0;
  int vel_cur = d_vel_inc->getValue();
  if (vel > vel_cur)
    {
      vel_diff = vel - vel_cur;
    }
  else
    {
      vel_diff = vel_cur - vel;
    }

  if (vel_diff < d_vel_inc->getIncRes()) // avoid stepping oscillation
    {
      return vel;
    }
  
  d_state->stepVelocity(*this, vel);
  updateState();
  
  return d_vel_inc->getValue();
}

//----------------------------------------------------------------------//
void RCStepVelocityManager::decrementVelocity()
{
  assert(d_vel_inc != nullptr);
  d_vel_inc->stepDecrement();
}
//----------------------------------------------------------------------//
void RCStepVelocityManager::incrementVelocity()
{
  assert(d_vel_inc != nullptr);
  d_vel_inc->stepIncrement();
}

//----------------------------------------------------------------------//
// ZeroState
//----------------------------------------------------------------------//
void ZeroVelocityState::stepVelocity(RCStepVelocityManager& man, int in_vel)
{
  switch (UTIL::getSign(in_vel))
    {
    case UTIL::Sign::Pos:
      man.incrementVelocity();
      man.setNextState(VelocityStateId::Pos);
      break;

    case UTIL::Sign::Neg:
      man.decrementVelocity();
      man.setNextState(VelocityStateId::Neg);
      break;

    case UTIL::Sign::Zero:
      //should already have zero velocity so do nothing
      assert(man.getVelocity() == 0);
      man.setNextState(VelocityStateId::Zero);
      break;

    default:
      assert(false);
      break;
    }
}

//----------------------------------------------------------------------//
// PosState
//----------------------------------------------------------------------//
void PosVelocityState::stepVelocity(RCStepVelocityManager& man, int in_vel)
{
  int cur_velocity = man.getVelocity();

  switch (UTIL::getSign(in_vel))
    {
    case UTIL::Sign::Pos:
      if (in_vel > cur_velocity)
	{
	  man.incrementVelocity();
	}
      else if (in_vel < cur_velocity)
	{
	  man.setVelocity(in_vel);
	}
      man.setNextState(VelocityStateId::Pos);
      break;

    case UTIL::Sign::Neg:
      man.setVelocity(0); // zero the set velocity
      man.decrementVelocity();
      man.setNextState(VelocityStateId::Neg);
      break;

    case UTIL::Sign::Zero:
      man.setVelocity(0);
      man.setNextState(VelocityStateId::Zero);
      break;

    default:
      assert(false);
      break;
    }
}

//----------------------------------------------------------------------//
void NegVelocityState::stepVelocity(RCStepVelocityManager& man, int in_vel)
{
  int cur_velocity = man.getVelocity();
  
  switch (UTIL::getSign(in_vel))
    {
    case UTIL::Sign::Pos:
      man.setVelocity(0);
      man.incrementVelocity();
      man.setNextState(VelocityStateId::Pos);
      break;

    case UTIL::Sign::Neg:
      if (in_vel < cur_velocity)
	{
	  man.decrementVelocity();
	}
      else if (in_vel > cur_velocity)
	{
	  man.setVelocity(in_vel);
	}
      man.setNextState(VelocityStateId::Neg);
      break;

    case UTIL::Sign::Zero:
      man.setVelocity(0);
      man.setNextState(VelocityStateId::Zero);
      break;

    default:
      assert(false);
      break;
    }
}

//----------------------------------------------------------------------//
int RCStepVelocityManager::getVelocity()
{
  return d_vel_inc->getValue();
}

//----------------------------------------------------------------------//
void RCStepVelocityManager::setVelocity(int vel)
{
  d_vel_inc->setValue(vel);
}
