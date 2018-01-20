#ifndef PWP_LEVELINPUTPIN
#define PWP_LEVELINPUTPIN

#include "wp_inputpin.h"

namespace P_WP
{
  // Triggers while the pin state is a specific value
  class LevelInputPin
  {
  public:
    enum class LevelValue
    {
      High,
	Low,
	Both // always triggering
    };
    
  public:
    LevelInputPin(int pin_num,
		  PullMode,
		  LevelValue);
    LevelInputPin(const LevelInputPin&) = delete;
    LevelInputPin& operator=(const LevelInputPin&) = delete;
    LevelInputPin(const LevelInputPin&&) = delete;
    LevelInputPin& operator=(const LevelInputPin&&) = delete;

    void setTriggerCallback(std::function<void(bool)>);
    
  private:
    bool hasTriggered();

  private:
    InputPin d_input_pin;
    
    LevelValue d_trigger_value = LevelValue::Both;
  };
};

#endif
