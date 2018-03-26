#ifndef PINEDGEREQUEST_H
#define PINEDGEREQUEST_H

#include "stateedgerequest.h"
#include "kn_asiocallbacktimer.h"
#include "wp_edgeinputpin.h"

class PinEdgeRequest final : StateEdgeRequest
{
 public:
  PinEdgeRequest(StateEdgeRequest::Owner* o,
		 int num,
		 P_WP::PullMode pull_mode);

 private:
  std::unique_ptr<P_WP::EdgeInputPin> d_pin;
};

#endif
