#include "wp_levelinputpin.h"

using namespace P_WP;

//----------------------------------------------------------------------//
LevelInputPin::LevelInputPin(PinNum pin_num,
			     PullMode pull,
			     LevelValue v)
  : InputPin(pin_num, pull),
    d_trigger_value(v)
{}

//----------------------------------------------------------------------//
bool LevelInputPin::hasTriggered()
{
  d_state = readState();

  switch (d_trigger_value)
    {
    case LevelValue::High:
      if (d_state) {return true;} break;

    case LevelValue::Low:
      if (!d_state) {return true;} break;

    case LevelValue::Both:
      return true;

    default:
      assert(false);
      break;
    }
  
  return false;
}
