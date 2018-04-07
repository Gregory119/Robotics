#ifndef PINEDGEREQUEST_H
#define PINEDGEREQUEST_H

#include "stateedgerequest.h"
#include "kn_asiocallbacktimer.h"
#include "wp_edgeinputpin.h"

class PinEdgeRequest final : public StateEdgeRequest
{
 public:
  PinEdgeRequest(StateEdgeRequest::Owner* o,
		 int num,
		 P_WP::PullMode pull_mode);

  void start() override;
  
 private:
  std::unique_ptr<P_WP::EdgeInputPin> d_pin;
};

#endif
