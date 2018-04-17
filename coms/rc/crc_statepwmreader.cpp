#include "crc_statepwmreader.h"

#include <iostream>

using namespace C_RC;

//----------------------------------------------------------------------//
StatePwmReader::StatePwmReader(Owner* o,
			       P_WP::PinNum pin,
			       std::chrono::microseconds max_pulse_duration,
			       std::chrono::microseconds min_pulse_duration)
  : d_owner(o)
{
  const PwmLimits<unsigned> limits(2000,
				   0,
				   max_pulse_duration,
				   min_pulse_duration);
  d_reader.reset(new PwmReader<unsigned>(this,
					 pin,
					 limits));

  d_state_detector.reset(new PwmStateChangeDetector<unsigned>(d_reader->getMaxVal(),
							      d_reader->getMinVal(),
							      [this](bool state){
								d_owner.call(&StatePwmReader::Owner::handleState, this, state);
							      }));
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
