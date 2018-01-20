#include "wp_levelinputpin.h"

using namespace P_WP;

//----------------------------------------------------------------------//
LevelInputPin::LevelInputPin(int pin_num,
			     PullMode pull,
			     LevelValue v)
  : InputPin(pin_num, pull),
    d_trigger_value(v)
{}

//----------------------------------------------------------------------//
bool LevelInputPin::hasTriggered()
{
  bool state = readState();

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
  
  d_state = state;
  return false;
}
