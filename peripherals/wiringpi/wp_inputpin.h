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
      Down,
      None
  };

  // Designed to be a base class that provides general input pin functionality.
  class InputPin
  {      
  public:
    InputPin(PinNum,
	     PullMode pull);
    InputPin(const InputPin&) = delete;
    InputPin& operator=(const InputPin&) = delete;
    InputPin(const InputPin&&) = delete;
    InputPin& operator=(const InputPin&&) = delete;
    
    void setTriggerCallback(std::function<void(bool)>); // set function to be called when a trigger is detected
    void setUpdateInterval(std::chrono::milliseconds delay =
			   std::chrono::milliseconds(50));

    void setPullMode(PullMode);
    bool readState(); // reads and returns the current physical pin state, but does not update the saved pin state
    bool getSavedState(); // gets the cached pin state

  protected:
    ~InputPin() = default; // must be inherited but not used polymorphically
    void updateState(); // reads the physical pin state and updates the saved pin state
    void setSavedState(bool state) { d_state = state;}
    
  protected:
    bool d_state = false;

  private:
    virtual bool hasTriggered() = 0;
    
  private:
    int d_num = static_cast<int>(PinNum::H11);
    PullMode d_pull_mode = PullMode::Down;
    
    KERN::AsioCallbackTimer d_check_state = KERN::AsioCallbackTimer("P_WP::InputPin - trigger checking timer");
    std::function<void(bool)> d_trigger;
  };
};

#endif