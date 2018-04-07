#ifndef PPMEDGEREQUEST_H
#define PPMEDGEREQUEST_H

#include "stateedgerequest.h"
#include "crc_stateppmreader.h"

class PpmEdgeRequest final : public StateEdgeRequest,
  C_RC::StatePpmReader::Observer
{
 public:
  PpmEdgeRequest(StateEdgeRequest::Owner* o,
		 int num,
		 C_RC::StatePpmReader*);

  void start() override;
  
 private:
  // StatePpmReader::Observer
  void handleState(C_RC::PpmReader<unsigned>*, bool state) override;
  void handleError(C_RC::PpmReader<unsigned>*,
		   C_RC::PpmReaderError,
		   const std::string& msg) override;
  unsigned getChannel() const override { return d_channel; }

 private:
  const unsigned d_channel = 0;
  C_RC::StatePpmReader* d_reader = nullptr;
};

#endif
