#ifndef PWP_INPUTPIN
#define PWP_INPUTPIN

#include "wp_pins.h"
#include "kn_asiocallbacktimer.h"

namespace P_WP
{
  class LevelInputPin;
  class EdgeInputPin;
  
  using PinName = std::string;
    
  enum class PullMode
  {
    Up,
    Down
  };

  // Designed to be a base class that provides general input pin functionality.
  // !!!CREATE A PIN CLASS, WHICH THIS ONE INHERITS. THE PIN CLASS WILL SAVE A STATIC LIST OF THE PIN NUMBERS TO ENSURE THAT EACH PIN IS USED IN ONLY ONE WAY!!!
  // POSSIBLY RUN LOOPING CHECK IN THREAD, WHICH THEN WILL SIGNAL A TRIGGER, WHICH WILL THEN CREATE A SINGLESHOT ZERO TIMER IN THE MAIN THREAD TO CALL THE CALLBACK FUNCTION
  class InputPin final
  {      
  public:
    InputPin(int pin_num,
	     PullMode pull);
    InputPin(const InputPin&) = delete;
    InputPin& operator=(const InputPin&) = delete;
    InputPin(const InputPin&&) = delete;
    InputPin& operator=(const InputPin&&) = delete;
    
    void setTriggerCallback(std::function<void(bool)>); // set function to be called when a trigger is detected
    void setTriggerCheck(std::function<bool()>); // set function to check for a trigger    
    void setUpdateInterval(std::chrono::milliseconds delay =
			   std::chrono::milliseconds(50));

    bool readState(); // reads and returns the current physical pin state, but does not update the saved pin state
    bool getSavedState(); // gets the cached pin state

  protected:
    friend class LevelInputPin;
    friend class EdgeInputPin;
    void updateState(); // reads the physical pin state and updates the saved pin state
    void setSavedState(bool state) { d_state = state;}
    
  protected:
    bool d_state = false;

  private:
    int d_num = P_WP::PinH11;
    PullMode d_pull_mode = PullMode::Down;
    
    KERN::AsioCallbackTimer d_check_state = KERN::AsioCallbackTimer("P_WP::InputPin - trigger checking timer");
    std::function<void(bool)> d_trigger;
    std::function<bool()> d_trigger_check;
  };
};

#endif
