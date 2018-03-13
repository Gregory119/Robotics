#ifndef PWP_EDGEINPUTPIN_H
#define PWP_EDGEINPUTPIN_H

#include "wp_inputpin.h"

namespace P_WP
{
  // Triggers on a pin edge state change. Uses a polling method. Good for pin switches that have bounce noise.
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
    EdgeInputPin(EdgeInputPin&&) = delete;
   
  private:
    bool hasTriggered() override;
    
  private:
    bool d_detected_change = false;
    EdgeType d_edge_type = EdgeType::Both;
  };
};

#endif
