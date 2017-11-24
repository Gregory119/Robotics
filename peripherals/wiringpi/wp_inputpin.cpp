#include "wp_inputpin.h"

#include <cassert>
#include <wiringPi.h>

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

  readState(); // read initial state
}

//----------------------------------------------------------------------//
void InputPin::setStateChangedCallback(std::function<void(bool)> func)
{
  d_callback = func;
}

//----------------------------------------------------------------------//
void InputPin::checkStateInterval(std::chrono::milliseconds delay)
{
#ifndef RELEASE
  if (!d_callback)
    {
      assert(false);
      return;
    }
#endif
  
  d_check_state.setCallback([this](){
      if (hasStateChanged())
	{
	  d_callback(d_state);
	}
    });
  d_check_state.restart(delay);
}

//----------------------------------------------------------------------//
void InputPin::readState()
{
  d_state = digitalRead(d_num);
}

//----------------------------------------------------------------------//
bool InputPin::hasStateChanged()
{
  bool state = digitalRead(d_num);
  if (d_state != state)
    {
      // previously detected change and the change still exists
      if (d_detected_change == true)
	{
	  d_detected_change = false; // reset the detection
	  d_state = state;
	  return true;
	}
      d_detected_change = true;
      return false;
    }
  d_detected_change = false; // reset the detection
  return false;
}
