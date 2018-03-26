#include "requestmanager.h"

#include "pinedgerequest.h"
#include "pwmedgerequest.h"
#include "ppmedgerequest.h"

//----------------------------------------------------------------------//
RequestManager::RequestManager(Owner* o, std::string config_file_path)
  : d_owner(o),
    d_config(new Config(this, std::move(config_file_path)))
{}

//----------------------------------------------------------------------//
void RequestManager::start()
{
  switch (d_config->getReqMode(Config::Request::Mode))
  d_mode_req.reset(new PinEdgeRequest(this, d_config->getReqNum(Config::Request::Mode)));

  /*
      if (!isValidPullMode(req_mode))
    {
      std::ostringstream stream("PinEdgeRequest::PinEdgeRequest - The request mode '",
				std::ios_base::app);
      stream << static_cast<int>(req_mode) << "' is not a valid pull mode. The request number is '"
	     << num << "'.";
      d_err_msg = stream.str();
      d_error_timer.singleShotZero();
      return;
    }

   */
  
  // Pins
  d_mode_pin.reset(new P_WP::EdgeInputPin(d_config->getReqNum(Config::Request::Mode),
					  d_config->getReqMode(Config::Request::Mode),
					  P_WP::EdgeInputPin::EdgeType::Both));
  d_trigger_pin.reset(new P_WP::EdgeInputPin(d_config->getReqNum(Config::Request::Trigger),
					     d_config->getReqMode(Config::Request::Trigger),
					     P_WP::EdgeInputPin::EdgeType::Both));
  d_power_pin.reset(new P_WP::EdgeInputPin(d_config->getReqNum(Config::Request::Power),
					   d_config->getReqMode(Config::Request::Power),
					   P_WP::EdgeInputPin::EdgeType::Both));
  d_mode_pin->setTriggerCallback([this](bool state){
      processModePinState(state);
    });
  d_trigger_pin->setTriggerCallback([this](bool state){
      processTriggerPinState(state);
    });
  d_power_pin->setTriggerCallback([this](bool state){
      processPowerPinState(state);
    });  
  d_mode_pin->setUpdateInterval(s_pin_update_time);
  d_trigger_pin->setUpdateInterval(s_pin_update_time);
  d_power_pin->setUpdateInterval(s_pin_update_time);
}

//----------------------------------------------------------------------//
void RequestManager::handleRequest(StateEdgeRequest*, DurationTrigger)
{
  assert(false);
  // STILLTODO!!
}

//----------------------------------------------------------------------//
void RequestManager::handleError(StateEdgeRequest*, const std::string&)
{
  assert(false);
  // STILLTODO!!
  //d_owner.call();
}
