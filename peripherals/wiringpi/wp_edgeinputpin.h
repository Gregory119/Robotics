#ifndef PWP_EDGEINPUTPIN
#define PWP_EDGEINPUTPIN

#include "wp_inputpin.h"

namespace P_WP
{
  // Triggers on an pin edge state change
  class EdgeInputPin final
  {
  public:
    enum class EdgeType
    {
      High,
	Low,
	Both
	};
    
  public:
    EdgeInputPin(int pin_num,
		 PullMode,
		 EdgeType);
    EdgeInputPin(const EdgeInputPin&) = delete;
    EdgeInputPin& operator=(const EdgeInputPin&) = delete;
    EdgeInputPin(const EdgeInputPin&&) = delete;
    EdgeInputPin& operator=(const EdgeInputPin&&) = delete;

    void setTriggerCallback(std::function<void(bool)>);
   
  private:
    bool hasTriggered();
    
  private:
    InputPin d_input_pin;
    
    bool d_detected_change = false;
    EdgeType d_edge_type = EdgeType::Both;
  };
};

#endif
