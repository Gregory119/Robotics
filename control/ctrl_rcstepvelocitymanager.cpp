#include "ctrl_rcstepvelocitymanager.h"
#include "core_stepincrementer.h"

#include "utl_values.h"

#include <cassert>

using namespace CTRL;

//----------------------------------------------------------------------//
// RCStepVelocityManager
//----------------------------------------------------------------------//
RCStepVelocityManager::RCStepVelocityManager(const VelocityLimitParams& p)
  : d_step_inc(new CORE::StepIncrementer(static_cast<double>(p.abs_max_velocity)/p.min_time_for_max_velocity_ms,
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
int RCStepVelocityManager::stepVelocity(int input_velocity)
{
  assert(d_state != nullptr);
  assert(d_step_inc != nullptr);
  
  // avoid oscillating set velocitys due to increment/decrement resolution
  if (d_step_inc->getIncRes() > std::abs(d_velocity - input_velocity))
    {
      return d_velocity;
    }
  
  d_state->stepVelocity(*this, input_velocity);
  updateState();
  
  return d_velocity;
}

//----------------------------------------------------------------------//
void RCStepVelocityManager::decrementVelocity()
{
  assert(d_step_inc != nullptr);
  d_velocity = d_step_inc->stepDecrement(d_velocity);
}
//----------------------------------------------------------------------//
void RCStepVelocityManager::incrementVelocity()
{
  assert(d_step_inc != nullptr);
  d_velocity = d_step_inc->stepIncrement(d_velocity);
}

//----------------------------------------------------------------------//
// ZeroState
//----------------------------------------------------------------------//
void ZeroVelocityState::stepVelocity(RCStepVelocityManager& man, int input_velocity)
{
  switch (UTIL::Values::getSign(input_velocity))
    {
    case UTIL::Values::Pos:
      man.incrementVelocity();
      man.setNextState(VelocityStateId::Pos);
      break;

    case UTIL::Values::Neg:
      man.decrementVelocity();
      man.setNextState(VelocityStateId::Neg);
      break;

    case UTIL::Values::Zero:
      //should already have zero velocity so do nothing
      assert(man.getVelocity() == 0);
      break;

    default:
      assert(false);
      break;
    }
}

//----------------------------------------------------------------------//
// PosState
//----------------------------------------------------------------------//
void PosVelocityState::stepVelocity(RCStepVelocityManager& man, int input_velocity)
{
  int cur_velocity = man.getVelocity();

  switch (UTIL::Values::getSign(input_velocity))
    {
    case UTIL::Values::Pos:
      if (input_velocity > cur_velocity)
	{
	  man.incrementVelocity();
	}
      else if (input_velocity < cur_velocity)
	{
	  man.setVelocity(input_velocity);
	}
      man.setNextState(VelocityStateId::Pos);
      break;

    case UTIL::Values::Neg:
      man.setVelocity(0);
      man.decrementVelocity();
      man.setNextState(VelocityStateId::Neg);
      break;

    case UTIL::Values::Zero:
      man.setVelocity(0);
      break;

    default:
      assert(false);
      break;
    }
}

//----------------------------------------------------------------------//
void NegVelocityState::stepVelocity(RCStepVelocityManager& man, int input_velocity)
{
  int cur_velocity = man.getVelocity();
  
  switch (UTIL::Values::getSign(input_velocity))
    {
    case UTIL::Values::Pos:
      man.setVelocity(0);
      man.incrementVelocity();
      man.setNextState(VelocityStateId::Pos);
      break;

    case UTIL::Values::Neg:
      if (input_velocity < cur_velocity)
	{
	  man.decrementVelocity();
	}
      else if (input_velocity > cur_velocity)
	{
	  man.setVelocity(input_velocity);
	}
      man.setNextState(VelocityStateId::Neg);
      break;

    case UTIL::Values::Zero:
      man.setVelocity(0);
      break;

    default:
      assert(false);
      break;
    }
}
