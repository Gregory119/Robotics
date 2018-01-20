#include "wp_inputpin.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

using namespace P_WP;

//----------------------------------------------------------------------//
InputPin::InputPin(int pin_num, PullMode pull)
  : d_num(pin_num),
    d_pull_mode(pull)
{
  pinMode(d_num, INPUT);
  setPullMode(pull);
  updateState(); // read initial state
}

//----------------------------------------------------------------------//
void InputPin::setPullMode(PullMode pull)
{
  switch (pull)
    {
    case PullMode::Up:
      pullUpDnControl(d_num, PUD_UP);
      break;
      
    case PullMode::Down:
      pullUpDnControl(d_num, PUD_DOWN);
      break;

    case PullMode::None:
      pullUpDnControl(d_num, PUD_OFF);
      break;

    default:
      assert(false);
      break;
    }
}

//----------------------------------------------------------------------//
void InputPin::setTriggerCallback(std::function<void(bool)> f)
{
  d_trigger = f;
}

//----------------------------------------------------------------------//
void InputPin::setUpdateInterval(std::chrono::milliseconds delay)
{
  if (!d_trigger)
    {
      assert(false);
      return;
    }
    
  d_check_state.setTimeoutCallback([this](){
      if (hasTriggered())
	{
	  d_trigger(d_state);
	}
    });
  d_check_state.restart(delay);
}

//----------------------------------------------------------------------//
void InputPin::updateState()
{
  d_state = digitalRead(d_num);
}

//----------------------------------------------------------------------//
bool InputPin::readState()
{
  return digitalRead(d_num);
}
