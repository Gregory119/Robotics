#ifndef STATEEDGEREQUESTFACTORY_H
#define STATEEDGEREQUESTFACTORY_H

#include "pinedgerequest.h"
#include "pwmedgerequest.h"
#include "ppmedgerequest.h"

#include "wp_pins.h"

using PpmPinNum = P_WP::PinNum;

class StateEdgeRequestFactory
{
  struct Params
  {
    StateEdgeRequest::Owner* o = nullptr;
    int req_num = -1;
    P_WP::PullMode pull_mode = P_WP::PullMode::None;
    C_RC::PwmLimitsType pwm_limits_type = C_RC::PwmLimitsType::Narrow;
    C_RC::StatePpmReader* state_ppmreader = nullptr;
  };
  
  std::unique_ptr<StateEdgeRequest> create(const Params&);
};

#endif
