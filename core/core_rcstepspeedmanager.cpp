#include "core_rcstepspeedmanager.h"
#include "core_stepincrementer.h"

#include "utl_values.h"

#include <cassert>

using namespace CORE;

//----------------------------------------------------------------------//
// RCStepSpeedManager
//----------------------------------------------------------------------//
RCStepSpeedManager::RCStepSpeedManager(const SpeedLimitParams& p)
{
  d_step_inc.reset(new StepIncrementer(p.abs_max_speed/p.min_time_for_max_speed_ms,
				       p.time_step_ms));
  
  // add list of possible states
  d_states[SpeedStateId::Pos] = std::unique_ptr<SpeedState>(new PosSpeedState());
  d_states[SpeedStateId::Neg] = std::unique_ptr<SpeedState>(new NegSpeedState());
  d_states[SpeedStateId::Zero] = std::unique_ptr<SpeedState>(new ZeroSpeedState());
  d_state = d_states[SpeedStateId::Zero].get();
}

//----------------------------------------------------------------------//
void RCStepSpeedManager::updateState()
{
  d_state = d_states[d_next_state_id].get();
  assert(d_state!=nullptr);
}

//----------------------------------------------------------------------//
void RCStepSpeedManager::setNextState(SpeedStateId id)
{
  d_next_state_id = id;
}

//----------------------------------------------------------------------//
int RCStepSpeedManager::stepSpeed(int input_speed)
{
  assert(d_state != nullptr);
  assert(d_step_inc != nullptr);
  
  // avoid oscillating set speeds due to increment/decrement resolution
  if (d_step_inc->getIncRes() > std::abs(d_speed - input_speed))
    {
      return d_speed;
    }
  
  d_state->stepSpeed(*this, input_speed);
  updateState();
  
  return d_speed;
}

//----------------------------------------------------------------------//
void RCStepSpeedManager::decrementSpeed()
{
  assert(d_step_inc != nullptr);
  d_speed = d_step_inc->stepDecrement(d_speed);
}
//----------------------------------------------------------------------//
void RCStepSpeedManager::incrementSpeed()
{
  assert(d_step_inc != nullptr);
  d_speed = d_step_inc->stepIncrement(d_speed);
}

//----------------------------------------------------------------------//
// ZeroState
//----------------------------------------------------------------------//
void ZeroSpeedState::stepSpeed(RCStepSpeedManager& man, int input_speed)
{
  switch (UTIL::Values::getSign(input_speed))
    {
    case UTIL::Values::Pos:
      man.incrementSpeed();
      man.setNextState(SpeedStateId::Pos);
      break;

    case UTIL::Values::Neg:
      man.decrementSpeed();
      man.setNextState(SpeedStateId::Neg);
      break;

    case UTIL::Values::Zero:
      //should already have zero speed so do nothing
      assert(man.getSpeed() == 0);
      break;

    default:
      assert(false);
      break;
    }
}

//----------------------------------------------------------------------//
// PosState
//----------------------------------------------------------------------//
void PosSpeedState::stepSpeed(RCStepSpeedManager& man, int input_speed)
{
  int cur_speed = man.getSpeed();

  switch (UTIL::Values::getSign(input_speed))
    {
    case UTIL::Values::Pos:
      if (input_speed > cur_speed)
	{
	  man.incrementSpeed();
	}
      else if (input_speed < cur_speed)
	{
	  man.setSpeed(input_speed);
	}
      man.setNextState(SpeedStateId::Pos);
      break;

    case UTIL::Values::Neg:
      man.setSpeed(0);
      man.decrementSpeed();
      man.setNextState(SpeedStateId::Neg);
      break;

    case UTIL::Values::Zero:
      man.setSpeed(0);
      break;

    default:
      assert(false);
      break;
    }
}

//----------------------------------------------------------------------//
void NegSpeedState::stepSpeed(RCStepSpeedManager& man, int input_speed)
{
  int cur_speed = man.getSpeed();
  
  switch (UTIL::Values::getSign(input_speed))
    {
    case UTIL::Values::Pos:
      man.setSpeed(0);
      man.incrementSpeed();
      man.setNextState(SpeedStateId::Pos);
      break;

    case UTIL::Values::Neg:
      if (input_speed < cur_speed)
	{
	  man.decrementSpeed();
	}
      else if (input_speed > cur_speed)
	{
	  man.setSpeed(input_speed);
	}
      man.setNextState(SpeedStateId::Neg);
      break;

    case UTIL::Values::Zero:
      man.setSpeed(0);
      break;

    default:
      assert(false);
      break;
    }
}
