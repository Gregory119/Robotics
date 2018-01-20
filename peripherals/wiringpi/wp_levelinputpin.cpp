#include "wp_levelinputpin.h"

using namespace P_WP;

//----------------------------------------------------------------------//
LevelInputPin::LevelInputPin(int pin_num,
			     PullMode pull,
			     LevelValue v)
  : d_input_pin(pin_num, pull),
    d_trigger_value(v)
{
  d_input_pin.setTriggerCheck([this](){
      return hasTriggered();
    });
}

//----------------------------------------------------------------------//
void LevelInputPin::setTriggerCallback(std::function<void(bool)> f)
{
  d_input_pin.setTriggerCallback(f);
}

//----------------------------------------------------------------------//
bool LevelInputPin::hasTriggered()
{
  bool state = d_input_pin.readState();

  switch (d_trigger_value)
    {
    case LevelValue::High:
      if (state) {return true;} break;

    case LevelValue::Low:
      if (!state) {return true;} break;

    case LevelValue::Both:
      return true;

    default:
      assert(false);
      break;
    }
  
  d_input_pin.setSavedState(state);
  return false;
}
