#include "crc_statepwmreader.h"

#include <iostream>

using namespace C_RC;

//----------------------------------------------------------------------//
StatePwmReader::StatePwmReader(Owner* o,
			       P_WP::PinNum pin,
			       PwmLimitsType type)
  : d_owner(o),
    d_reader(new PwmReader<unsigned>(this,
				     pin,
				     PwmLimits<unsigned>::create(type, 2000, 0))),
    d_state_detector(new PwmStateChangeDetector<unsigned>(d_reader->getMaxVal(),
							  d_reader->getMinVal(),
							  [this](bool state){
							    d_owner.call(&StatePwmReader::Owner::handleState, this, state);
							  }))
{
  d_reader->capData();
}

//----------------------------------------------------------------------//
void StatePwmReader::handleValue(PwmReader<unsigned>*, unsigned val)
{
  d_state_detector->checkValue(val);
}

//----------------------------------------------------------------------//
void StatePwmReader::handleValueOutOfRange(PwmReader<unsigned>*, unsigned)
{
  assert(false);
}

//----------------------------------------------------------------------//
void StatePwmReader::handleError(PwmReader<unsigned>*,
				 PwmReaderError e,
				 const std::string& msg)
{
  d_owner.call(&StatePwmReader::Owner::handleError, this, e, msg);
}
