#ifndef PWMEDGEREQUEST_H
#define PWMEDGEREQUEST_H

#include "stateedgerequest.h"
#include "kn_asiocallbacktimer.h"
#include "crc_statepwmreader.h"

#include <chrono>

class PwmEdgeRequest final : public StateEdgeRequest,
  C_RC::StatePwmReader::Owner
{
 public:
  // the PwmLimits max and min vals are changed internally, so any number can be used for these
  PwmEdgeRequest(StateEdgeRequest::Owner* o,
		 int num,
		 std::chrono::microseconds max_pulse_duration,
		 std::chrono::microseconds min_pulse_duration);

  void start() override { d_reader->start(); }
  void stop() override { d_reader->stop(); }
  
 private:
  void handleState(C_RC::StatePwmReader*, bool) override;
  void handleError(C_RC::StatePwmReader*,
		   C_RC::PwmReaderError,
		   const std::string& msg) override;
  
 private:
  std::unique_ptr<C_RC::StatePwmReader> d_reader;
};

#endif
