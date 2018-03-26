#ifndef PPMEDGEREQUEST_H
#define PPMEDGEREQUEST_H

#include "stateedgerequest.h"
#include "crc_stateppmreader.h"

class PpmEdgeRequest final : StateEdgeRequest,
  C_RC::StatePpmReader::Observer
{
 public:
  PpmEdgeRequest(StateEdgeRequest::Owner* o,
		 int req_num,
		 C_RC::StatePpmReader*);

 private:
  // StatePpmReader::Observer
  void handleState(PpmReader<unsigned>*, bool state) override;
  unsigned getChannel() override { return d_channel; }

 private:
  const unsigned d_channel = 0;
};

#endif
