#include "pwmedgerequest.h"

//----------------------------------------------------------------------//
PwmEdgeRequest::PwmEdgeRequest(StateEdgeRequest::Owner* o,
			       int num,
			       std::chrono::microseconds max_pulse_duration,
			       std::chrono::microseconds min_pulse_duration)
  : StateEdgeRequest(o)
{
  if (!P_WP::PIN_UTILS::isNumInPinRange(num))
    {
      std::ostringstream stream("PwmEdgeRequest::PwmEdgeRequest - The request number of '",
				std::ios_base::app);
      stream << num << "' is not a valid pin number.";
      d_err_msg = stream.str();
      d_error_timer.singleShotZero([&](){
	  ownerError(d_err_msg);
	});
      return;
    }
  
  d_reader.reset(new C_RC::StatePwmReader(this,
					  static_cast<P_WP::PinNum>(num),
					  max_pulse_duration,
					  min_pulse_duration));
}

//----------------------------------------------------------------------//
void PwmEdgeRequest::start()
{
  d_reader->start();
}

//----------------------------------------------------------------------//
void PwmEdgeRequest::handleState(C_RC::StatePwmReader*, bool state)
{
  processState(state);
}

//----------------------------------------------------------------------//
void PwmEdgeRequest::handleError(C_RC::StatePwmReader*,
				 C_RC::PwmReaderError,
				 const std::string& msg)
{
  ownerError(msg);
}
