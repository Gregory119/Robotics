#include "requestmanager.h"

#include "pinedgerequest.h"
#include "pwmedgerequest.h"
#include "ppmedgerequest.h"

//----------------------------------------------------------------------//
RequestManager::RequestManager(Owner* o,
			       std::string config_file_path)
  : d_owner(o),
    d_config(new Config(this, std::move(config_file_path)))
{}

//----------------------------------------------------------------------//
void RequestManager::start()
{
  if (d_started)
    {
      return;
    }
  d_started = true;
  
  d_config->parseFile(); // errors handled in error callback, which are passed to the owner

  Config::ReqConfig next_mode_config = d_config->getReqConfig(Config::Request::NextMode);
  if (next_mode_config.isValid())
    {
      Config::ReqConfig trigger_config = d_config->getReqConfig(Config::Request::Trigger);
      if (trigger_config.mode == next_mode_config.mode &&
	  trigger_config.num == next_mode_config.num)
	{
	  d_mode = Mode::CombinedModeAndTrigger;
	}
      else
	{
	  d_mode = Mode::SeparateModeAndTrigger;
	}
    }
  else
    {
      d_mode = Mode::CombinedModeAndTrigger;
    }
  
  d_power_req = createRequest(Config::Request::Power);
  if (d_power_req == nullptr)
    {
      // error already sent from inside createRequest
      return;
    }
  // enable shutdown and restart requests using the same power request
  d_power_req->setTrigger(StateEdgeRequest::TriggerSetting::Combination, // enable on off and after duration triggers
			  std::chrono::seconds(5)); // duration
  d_power_req->start();

  switch (d_mode)
    {
    case Mode::SeparateModeAndTrigger:
      d_next_mode_req = createRequest(Config::Request::NextMode);
      d_trigger_req = createRequest(Config::Request::Trigger);
      if (d_trigger_req == nullptr ||
	  d_next_mode_req == nullptr)
	{
	  // error already sent from inside createRequest
	  return;
	}
      d_next_mode_req->start();
      d_trigger_req->start();
      return;

    case Mode::CombinedModeAndTrigger:
      // handle next mode and trigger requests using the mode request
      d_trigger_req = createRequest(Config::Request::Trigger);
      if (d_trigger_req == nullptr)
	{
	  // error already sent from inside createRequest
	  return;
	}
      d_trigger_req->setTrigger(StateEdgeRequest::TriggerSetting::Combination, // enable on off and after duration triggers
				std::chrono::seconds(1)); // duration
      d_trigger_req->start();
      return;
    }
  assert(false);
  // should not happen because d_mode is set above
}

//----------------------------------------------------------------------//
void RequestManager::handleError(Config*,
				 Config::Error,
				 const std::string& msg)
{
  std::string new_msg = "handleError(Config*..) - Failed with the following error:\n";
  new_msg += msg;
  ownerError(Error::Config, new_msg);
}

//----------------------------------------------------------------------//
void RequestManager::handleRequest(StateEdgeRequest* state_edge_request,
				   StateEdgeRequest::Trigger trigger)
{
  assert(state_edge_request != nullptr);
  // handle a power request
  if (state_edge_request == d_power_req.get())
    {
      switch (trigger)
	{
	case StateEdgeRequest::Trigger::None:
	  assert(false);
	  return;
	  
	case StateEdgeRequest::Trigger::AfterDuration:
	  d_owner.call(&Owner::handleReqShutdown, this);
	  return;
	  
	case StateEdgeRequest::Trigger::OnOffBeforeDuration:
	  d_owner.call(&Owner::handleReqReboot, this);
	  return;
	}
      // a callback should not provide the wrong trigger enum
      assert(false);
      return;
    }
  
  // handle next mode and trigger requests
  switch (d_mode)
    {
    case Mode::SeparateModeAndTrigger:
      if (state_edge_request == d_next_mode_req.get())
	{
	  d_owner.call(&Owner::handleReqNextMode, this);
	  return;
	}

      if (state_edge_request == d_trigger_req.get())
	{
	  d_owner.call(&Owner::handleReqTrigger, this);
	  return;
	}
      return;

    case Mode::CombinedModeAndTrigger:
      if (state_edge_request == d_trigger_req.get())
	{
	  switch (trigger)
	    {
	    case StateEdgeRequest::Trigger::None:
	      assert(false);
	      return;
	  
	    case StateEdgeRequest::Trigger::AfterDuration:
	      d_owner.call(&Owner::handleReqNextMode, this);
	      return;
	  
	    case StateEdgeRequest::Trigger::OnOffBeforeDuration:
	      d_owner.call(&Owner::handleReqTrigger, this);
	      return;
	    }
	  // a callback should not provide the wrong trigger enum
	  assert(false);
	}
      return;
    }
  assert(false);
  // this should not happen because this condition is handled in the start function, which will then activate this funciton
}

//----------------------------------------------------------------------//
void RequestManager::handleError(StateEdgeRequest* state_edge_request,
				 const std::string& msg)
{
  std::string new_msg = "handleError(StateEdgeRequest*..) - The ";
  if (state_edge_request == d_next_mode_req.get())
    {
      new_msg += "Next Mode request failed with the following error:\n";
      new_msg += msg;
      ownerError(Error::NextModeReq, new_msg);
    }
  else if (state_edge_request == d_trigger_req.get())
    {
      new_msg += "Trigger request failed with the following error:\n";
      new_msg += msg;
      ownerError(Error::TriggerReq, new_msg);
    }
  else if (state_edge_request == d_power_req.get())
    {
      new_msg += "Power request failed with the following error:\n";
      new_msg += msg;
      ownerError(Error::PowerReq, new_msg);
    }
  else
    {
      assert(false);
      return;
    }
}

//----------------------------------------------------------------------//
std::unique_ptr<StateEdgeRequest>
RequestManager::createRequest(Config::Request config_req)
{
  ReqMode req_mode = d_config->getReqMode(config_req);
  switch (req_mode)
    {
    case ReqMode::Up:
    case ReqMode::Down:
    case ReqMode::Float:
      {
	P_WP::PullMode pull_mode = P_WP::PullMode::Up;
	if (!Config::getPullMode(req_mode, pull_mode))
	  {
	    assert(false);
	    std::ostringstream stream("createRequest - Failed to convert the request mode ",
				      std::ios_base::app);
	    stream << static_cast<int>(req_mode) << " to a pull mode.";
	    ownerError(Error::ConvertReqModeToPullMode, stream.str());
	    return std::unique_ptr<StateEdgeRequest>();;
	  }
	return std::unique_ptr<StateEdgeRequest>(new PinEdgeRequest(this,
								    d_config->getReqNum(config_req),
								    pull_mode));
      }

      // use config to specify the pwm limits
    case ReqMode::Pwm:
      return std::unique_ptr<StateEdgeRequest>(new PwmEdgeRequest(this,
								  d_config->getReqNum(config_req),
								  std::chrono::microseconds(d_config->getPwmHighUs()),
								  std::chrono::microseconds(d_config->getPwmLowUs())));
      
    case ReqMode::Ppm:
      if (d_stateppmreader == nullptr)
	{
	  d_stateppmreader.reset(new C_RC::StatePpmReader(d_config->getPpmPinNum(),
							  std::chrono::microseconds(d_config->getPwmHighUs()),
							  std::chrono::microseconds(d_config->getPwmLowUs())));
	}
      return std::unique_ptr<StateEdgeRequest>(new PpmEdgeRequest(this,
								  d_config->getReqNum(config_req),
								  d_stateppmreader.get()));
      
    case ReqMode::Unknown:
      assert(false);
      ownerError(Error::ReqMode, "createRequest - The ReqMode is unknown.");
      return std::unique_ptr<StateEdgeRequest>();
    };
  assert(false);
  std::ostringstream stream("createRequest - The ReqMode is invalid with a value of ",
			    std::ios_base::app);
  stream << static_cast<int>(req_mode) << ".";
  ownerError(Error::ReqMode, stream.str());
  return std::unique_ptr<StateEdgeRequest>();
}

//----------------------------------------------------------------------//
void RequestManager::ownerError(Error e, const std::string& msg)
{
  std::string new_msg = "RequestManager::";
  new_msg += msg;
  d_owner.call(&Owner::handleError, this, e, new_msg);
}
