#include "pwmedgerequest.h"

//----------------------------------------------------------------------//
PwmEdgeRequest::PwmEdgeRequest(StateEdgeRequest::Owner* o,
			       int num,
			       C_RC::PwmLimitsType type)
  : StateEdgeRequest(o)
{
  if (!P_WP::isNumInPinRange(num))
    {
      std::ostringstream stream("PwmEdgeRequest::PwmEdgeRequest - The request number of '",
				std::ios_base::app);
      stream << num << "' is not a valid pin number.";
      d_err_msg = stream.str();
      d_error_timer.singleShotZero();
      return;
    }
  
  d_pwm_reader.reset(new C_RC::StatePwmReader(this,
					      static_cast<P_WP::PinNum>(num),
					      type));
}

//----------------------------------------------------------------------//
void PwmEdgeRequest::handleState(StatePwmReader*, bool state)
{
  processState(state);
}

//----------------------------------------------------------------------//
void PwmEdgeRequest::handleError(StatePwmReader*,
				 PwmReaderError,
				 const std::string& msg)
{
  d_owner.call(&StateEdgeRequest::Owner::handleError, this, msg);
}
