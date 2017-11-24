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
  
  class InputPin final
  {      
  public:
    InputPin(int pin_num,
	     PullMode pull = PullMode::Down);
    void setStateChangedCallback(std::function<void(bool)>);
    
    // Possibly have this class inherit an interface that can be used to notify it of a pin state change using an interrupt.
    // In this case, the pin state will not need to be checked on an interval. Otherwise check the state change on an
    // interval with checkStateInterval(..).
    void checkStateInterval(std::chrono::milliseconds delay =
			    std::chrono::milliseconds(50));
    void readState(); // reads the pin state and updates it without checking for a change (does not call callback)
    bool getState() const { return d_state; }

  private:
    bool hasStateChanged(); // updates the pin state on a change
    
  private:
    int d_num = P_WP::PinH11;
    PullMode d_pull_mode = PullMode::Down;
    bool d_state = false;
    bool d_detected_change = false;

    KERN::AsioCallbackTimer d_check_state;
    std::function<void(bool)> d_callback;
  };
};

#endif
