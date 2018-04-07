#include "pinedgerequest.h"

//----------------------------------------------------------------------//
PinEdgeRequest::PinEdgeRequest(StateEdgeRequest::Owner* o,
			       int num,
			       P_WP::PullMode pull_mode)
  : StateEdgeRequest(o)
{
  assert(o!=nullptr);
  
  if (!P_WP::PIN_UTILS::isNumInPinRange(num))
    {
      std::ostringstream stream("PinEdgeRequest::PinEdgeRequest - The request number of '",
				std::ios_base::app);
      stream << num << "' is not a valid pin number.";
      d_err_msg = stream.str();
      d_error_timer.singleShotZero([&](){
	  ownerError(d_err_msg);
	});
      return;
    }

  d_pin.reset(new P_WP::EdgeInputPin(static_cast<P_WP::PinNum>(num),
				     pull_mode,
				     P_WP::EdgeInputPin::EdgeType::Both));
  d_pin->setTriggerCallback([this](bool state){
      processState(state);
    });
}

//----------------------------------------------------------------------//
void PinEdgeRequest::start()
{
  d_pin->start();
}
