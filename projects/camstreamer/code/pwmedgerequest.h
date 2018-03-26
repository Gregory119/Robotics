#ifndef PWMEDGEREQUEST_H
#define PWMEDGEREQUEST_H

#include "stateedgerequest.h"
#include "kn_asiocallbacktimer.h"
#include "crc_statepwmreader.h"

class PwmEdgeRequest final : StateEdgeRequest,
  C_RC::StatePwmReader::Owner
{
 public:
  PwmEdgeRequest(StateEdgeRequest::Owner* o,
		 int num,
		 C_RC::PwmLimitsType); // add a pointer to a pwm reader so that this can be attached to it as an observer

  void handleState(StatePwmReader*, bool) override;
  void handleError(StatePwmReader*,
		   PwmReaderError,
		   const std::string& msg) override;
  
 private:
  std::unique_ptr<C_RC::StatePwmReader> d_pwm_reader;
};

#endif
