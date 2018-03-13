#include "wp_edgeinputpin.h"

using namespace P_WP;

//----------------------------------------------------------------------//
EdgeInputPin::EdgeInputPin(PinNum pin_num,
			   PullMode pull,
			   EdgeType t)
  : InputPin(pin_num, pull),
    d_edge_type(t)
{}

//----------------------------------------------------------------------//
bool EdgeInputPin::hasTriggered()
{
  bool state = readState();
  if (d_state == state)
    {
      d_detected_change = false; // reset the detection
      return false;
    }

  // switch debouncing
  // detected a state change at this point
  if (d_detected_change == false)
    {
      d_detected_change = true;
      return false;
    }
      
  // state change still exists after previous detection
  d_detected_change = false; // reset after detection
  d_state = state;
  switch (d_edge_type)
    {
    case EdgeType::High:
      if (state) {return true;} break;

    case EdgeType::Low:
      if (!state) {return true;} break;

    case EdgeType::Both:
      return true;
    }
  
  return false;
}
