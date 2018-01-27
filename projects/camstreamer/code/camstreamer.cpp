#include "camstreamer.h"

#include <iostream>

static const std::chrono::milliseconds s_pin_update_time =
  std::chrono::milliseconds(25);

//----------------------------------------------------------------------//
CamStreamer::CamStreamer(const std::string& config_file_path)
{
  // Created as a class member variable to ensure handleError is called with an existing Config instance.
  d_config.reset(new Config(this, config_file_path)); 
  if (d_config->hasError())
    {
      // Logging will happen in handleError after this constructor.
      // Set the LED to indicate a settings error.
      return;
    }
  
  // Pins
  d_mode_pin.reset(new P_WP::EdgeInputPin(d_config->getPinNum(Config::PinId::Mode),
					  d_config->getPinPullMode(Config::PinId::Mode),
					  P_WP::EdgeInputPin::EdgeType::Both));
  d_trigger_pin.reset(new P_WP::EdgeInputPin(d_config->getPinNum(Config::PinId::Trigger),
					     d_config->getPinPullMode(Config::PinId::Trigger),
					     P_WP::EdgeInputPin::EdgeType::Both));
  d_connect_pin.reset(new P_WP::EdgeInputPin(d_config->getPinNum(Config::PinId::Connect),
					     d_config->getPinPullMode(Config::PinId::Connect),
					     P_WP::EdgeInputPin::EdgeType::Both));
  d_mode_pin->setTriggerCallback([this](bool state){
      processModePinState(state);
    });
  d_trigger_pin->setTriggerCallback([this](bool state){
      processTriggerPinState(state);
    });  
  d_connect_pin->setTriggerCallback([this](bool state){
      processConnectPinState(state);
    });
  d_mode_pin->setUpdateInterval(s_pin_update_time);
  d_trigger_pin->setUpdateInterval(s_pin_update_time);
  d_connect_pin->setUpdateInterval(s_pin_update_time);

  // extract GoPro type from xml (app setting)
  d_gpcont_params.setType(D_GP::CamModel::Hero5).setName("CamStreamer");
  restartGPController();
  
  d_reset_gp.setTimeoutCallback([this](){
      restartGPController();
    });
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(Config*, Config::Error e, const std::string& msg)
{
  std::cerr << msg << std::endl;
  d_delete_config.deletePtr(d_config); 
}

//----------------------------------------------------------------------//
void CamStreamer::restartGPController()
{
  d_gp_controller.reset(new D_GP::ModeController(this, d_gpcont_params));
  d_gp_controller->connect();
  d_gp_controller->startStream();
}

//----------------------------------------------------------------------//
void CamStreamer::processModePinState(bool)
{
  std::cout << "CamStreamer::processModePinState()" << std::endl;
  if (d_gp_controller == nullptr)
    {
      return;
    }
  d_gp_controller->nextMode();
}

//----------------------------------------------------------------------//
void CamStreamer::processConnectPinState(bool state)
{
  std::cout << "CamStreamer::processConnectPinState()" << std::endl;
  if (d_gp_controller == nullptr)
    {
      return;
    }
  
  if (state) // high edge
    {
      d_gp_controller->connect();
    }
}

//----------------------------------------------------------------------//
void CamStreamer::processTriggerPinState(bool)
{
  std::cout << "CamStreamer::processTriggerPinState()" << std::endl;
  if (d_gp_controller == nullptr)
    {
      return;
    }
  d_gp_controller->trigger();
}

//----------------------------------------------------------------------//
void CamStreamer::handleFailedRequest(D_GP::ModeController* ctrl,
				      D_GP::ModeController::Req req)
{
  std::cerr << "CamStreamer::handleFailedRequest" << std::endl;
  // disable any other callbacks
  ctrl->setOwner(nullptr);
  // delete on timeout 
  d_delete_gpcont.deletePtr(d_gp_controller); 
  d_reset_gp.singleShot(std::chrono::seconds(5));
  
  // led => display not connected
}

//----------------------------------------------------------------------//
void CamStreamer::handleSuccessfulRequest(D_GP::ModeController*,
					  D_GP::ModeController::Req)
{
  // led => display connected
}
