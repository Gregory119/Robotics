#ifndef PWP_INPUTPIN
#define PWP_INPUTPIN

#include "wp_pins.h"
#include "kn_asiocallbacktimer.h"

namespace P_WP
{
  using PinName = std::string;
    
  enum class PullMode
  {
    Up,
    Down
  };

  // Designed to be a base class that provides general input pin functionality.
  // !!!CREATE A PIN CLASS, WHICH THIS ONE INHERITS. THE PIN CLASS WILL SAVE A STATIC LIST OF THE PIN NUMBERS TO ENSURE THAT EACH PIN IS USED IN ONLY ONE WAY!!!
  // POSSIBLY RUN LOOPING CHECK IN THREAD, WHICH THEN WILL SIGNAL A TRIGGER, WHICH WILL THEN CREATE A SINGLESHOT ZERO TIMER IN THE MAIN THREAD TO CALL THE CALLBACK FUNCTION
  class InputPin
  {      
  public:
    InputPin(int pin_num,
	     PullMode pull = PullMode::Up);
    InputPin(const InputPin&) = delete;
    InputPin& operator=(const InputPin&) = delete;
    
    void setTriggerCallback(std::function<void(bool)>);
    
    // Possibly have this class inherit an interface that can be used to notify it of a pin state change using an interrupt.
    // In this case, the pin state will not need to be checked on an interval. Otherwise check the state change on an
    // interval with setUpdateInterval(..).
    void setUpdateInterval(std::chrono::milliseconds delay =
			   std::chrono::milliseconds(50));    

  protected:
    ~InputPin() = default; // not used polymorphically	
    void updateState(); // reads the physical pin state and updates the saved pin state
    bool readState(); // reads and returns the physical pin state, but does not update the saved pin state
    
  private:
    virtual bool hasTriggered() = 0;
    
  protected:
    bool d_state = false;

  private:
    int d_num = P_WP::PinH11;
    PullMode d_pull_mode = PullMode::Down;
    
    KERN::AsioCallbackTimer d_check_state = KERN::AsioCallbackTimer("P_WP::InputPin - trigger checking timer");
    std::function<void(bool)> d_callback;
  };
};

#endif
