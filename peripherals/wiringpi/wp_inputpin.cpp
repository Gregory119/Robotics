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

  switch (pull)
    {
    case PullMode::Down:
      pullUpDnControl(d_num, PUD_DOWN);
      break;

    case PullMode::Up:
      pullUpDnControl(d_num, PUD_UP);
      break;
    }

  updateState(); // read initial state
}

//----------------------------------------------------------------------//
void InputPin::setTriggerCallback(std::function<void(bool)> func)
{
  d_callback = func;
}

//----------------------------------------------------------------------//
void InputPin::setUpdateInterval(std::chrono::milliseconds delay)
{
#ifndef RELEASE
  if (!d_callback)
    {
      assert(false);
      return;
    }
#endif
    
  d_check_state.setTimeoutCallback([this](){
      if (hasTriggered())
	{
	  d_callback(d_state);
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
