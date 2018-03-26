#include "ppmedgerequest.h"

//----------------------------------------------------------------------//
PpmEdgeRequest::PpmEdgeRequest(StateEdgeRequest::Owner* o,
			       int req_num,
			       C_RC::StatePpmReader* ppm_reader)
  : StateEdgeRequest(o)
{
  // !!!!!!!!!!!check if req_num is a valid channel number
  assert(false); // STILLTODO
  d_channel = req_num;
  
  ppm_reader->attach(this);
}

//----------------------------------------------------------------------//
// StatePpmReader::Observer
//----------------------------------------------------------------------//
void PpmEdgeRequest::handleState(PpmReader<unsigned>*, bool state)
{
  processState(state);
}
