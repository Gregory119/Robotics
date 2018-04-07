#include "crc_stateppmreader.h"

using namespace C_RC;

//----------------------------------------------------------------------//
// StatePpmReader::Observer
//----------------------------------------------------------------------//
void StatePpmReader::Observer::handleValue(PpmReader<unsigned>* ppm_reader,
					   unsigned val)
{
  if (d_state_detector == nullptr)
    {
      d_state_detector.reset(new PwmStateChangeDetector<unsigned>(ppm_reader->getMaxVal(),
								  ppm_reader->getMinVal(),
								  [&](bool state){
								    handleState(ppm_reader, state);
								  }));
      ppm_reader->capData();
    }
  d_state_detector->checkValue(val);
}

//----------------------------------------------------------------------//
void StatePpmReader::Observer::handleValueOutOfRange(PpmReader<unsigned>* ppm_reader,
						     unsigned)
{
  assert(false);
}

//----------------------------------------------------------------------//
// StatePpmReader
//----------------------------------------------------------------------//
StatePpmReader::StatePpmReader(P_WP::PinNum pin,
			       std::chrono::microseconds max_pulse_duration,
			       std::chrono::microseconds min_pulse_duration)
{
  const PwmLimits<unsigned> limits(2000,
				   0,
				   max_pulse_duration,
				   min_pulse_duration);
  d_reader.reset(new PpmReader<unsigned>(pin,
					 limits));
  d_reader->capData();
}

//----------------------------------------------------------------------//
void StatePpmReader::start()
{
  d_reader->start();
}
  
//----------------------------------------------------------------------//
void StatePpmReader::attachObserver(Observer* ob)
{
  d_reader->attachObserver(ob);
}
