#ifndef PWP_LEVELINPUTPIN
#define PWP_LEVELINPUTPIN

#include "wp_inputpin.h"

namespace P_WP
{
  // Triggers on a change in level/state of the digital input pin to a specific value
  class LevelInputPin : public InputPin
  {
  public:
    // CREATE CONSTRUCTOR THAT SETS THE TRIGGER VALUE
    // EDIT hasTriggered() to return true depending on the trigger value
    // THE GOPRO STREAMER WILL NEED TO USE THIS CLASS FOR THE PIN CONTROL
    
    enum class LevelValue
    {
      High,
	Low,
	Both
    };
    
  private:
    bool hasTriggered() override
    {
      bool state = readState();
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

  private:
    bool d_detected_change = false;
    LevelValue d_trigger_value = LevelValue::Both;
  };
};

#endif
