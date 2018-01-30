#ifndef PWP_EDGEINPUTPIN
#define PWP_EDGEINPUTPIN

#include "wp_inputpin.h"

namespace P_WP
{
  // Triggers on an pin edge state change
  class EdgeInputPin final : public InputPin
  {
  public:
    enum class EdgeType
    {
      High,
	Low,
	Both
	};
    
  public:
    EdgeInputPin(PinNum,
		 PullMode,
		 EdgeType);
    EdgeInputPin(const EdgeInputPin&) = delete;
    EdgeInputPin& operator=(const EdgeInputPin&) = delete;
    EdgeInputPin(EdgeInputPin&&) = delete;
    EdgeInputPin& operator=(EdgeInputPin&&) = delete;
   
  private:
    bool hasTriggered() override;
    
  private:
    bool d_detected_change = false;
    EdgeType d_edge_type = EdgeType::Both;
  };
};

#endif
