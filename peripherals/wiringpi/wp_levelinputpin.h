#ifndef PWP_LEVELINPUTPIN
#define PWP_LEVELINPUTPIN

#include "wp_inputpin.h"

namespace P_WP
{
  // Triggers while the pin state is a specific value
  class LevelInputPin final : public InputPin
  {
  public:
    enum class LevelValue
    {
      High,
	Low,
	Both // always triggering
    };
    
  public:
    LevelInputPin(PinNum,
		  PullMode,
		  LevelValue);
    LevelInputPin(const LevelInputPin&) = delete;
    LevelInputPin& operator=(const LevelInputPin&) = delete;
    LevelInputPin(const LevelInputPin&&) = delete;
    LevelInputPin& operator=(const LevelInputPin&&) = delete;

  private:
    bool hasTriggered() override;

  private:
    LevelValue d_trigger_value = LevelValue::Both;
  };
};

#endif
