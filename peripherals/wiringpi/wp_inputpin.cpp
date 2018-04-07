#include "wp_inputpin.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

using namespace P_WP;

//----------------------------------------------------------------------//
InputPin::InputPin(PinNum pin_num, PullMode pull)
  : d_num(static_cast<int>(pin_num)),
    d_pull_mode(pull)
{
  pinMode(d_num, INPUT);
  PIN_UTILS::setPullMode(d_num, pull);
  updateState(); // read initial state
  setUpdateInterval();
}

//----------------------------------------------------------------------//
void InputPin::setTriggerCallback(std::function<void(bool)> f)
{
  d_trigger = f;
}

//----------------------------------------------------------------------//
void InputPin::setUpdateInterval(std::chrono::milliseconds delay)
{
  d_check_state.setTime(delay);
}

//----------------------------------------------------------------------//
void InputPin::start()
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
  d_check_state.restart();
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
