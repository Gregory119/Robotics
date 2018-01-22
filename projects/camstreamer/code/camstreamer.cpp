#include "camstreamer.h"

static const std::chrono::milliseconds s_pin_update_time =
  std::chrono::milliseconds(25);

//----------------------------------------------------------------------//
CamStreamer::CamStreamer()
{
  // If the file config extraction fails, then modify led (LED class) and return. Re-inserting the usb with correct config file settings should restart the software correctly.
  // extract pin numbers from xml
  // d_pin_mode_num = ??;
  // d_pin_trigger_num = ??;
  // pin functional name
  // pin header number

  // ARE SERVO RECEIVER OUTPUT PINS NORMALLY HIGH OR LOW??? SUPPORT OTHER RC RECEIVER TECHNOLOGIES (NEED SETUP ON THE APP)??
  // READ CONFIG TO DETERMINE NEEDED PIN PULL MODE (DIGITAL PIN INPUT OR SWITCH/BUTTON => NO PULL OR PULL UP)
  // d_mode_pin.setPullMode(...);
  // ...
  
  // Pins
  d_mode_pin.reset(new P_WP::EdgeInputPin(P_WP::PinNum::H11,
					  P_WP::PullMode::Up,
					  P_WP::EdgeInputPin::EdgeType::Both));
  d_trigger_pin.reset(new P_WP::EdgeInputPin(P_WP::PinNum::H13,
					     P_WP::PullMode::Up,
					     P_WP::EdgeInputPin::EdgeType::Both));
  d_connect_pin.reset(new P_WP::EdgeInputPin(P_WP::PinNum::H15,
					     P_WP::PullMode::Up,
					     P_WP::EdgeInputPin::EdgeType::Both));
  d_mode_pin.setTriggerCallback([this](bool state){
      processModePinState(state);
    });
  d_trigger_pin.setTriggerCallback([this](bool state){
      processTriggerPinState(state);
    });  
  d_connect_pin.setTriggerCallback([this](bool state){
      processConnectPinState(state);
    });
  d_mode_pin.setUpdateInterval(s_pin_update_time);
  d_trigger_pin.setUpdateInterval(s_pin_update_time);
  d_connect_pin.setUpdateInterval(s_pin_update_time);

  // extract gopro type from xml (app setting)
  d_gpcont_params.setType(D_GP::CamModel::Hero5).setName("CamStreamer");
  restartGPController();
  
  d_reset_gp.setTimeoutCallback([this](){
      restartGPController();
    });
}

//----------------------------------------------------------------------//
void CamStreamer::restartGPController()
{
  d_gp_controller.reset(new D_GP::ModeController(this,d_gpcont_params));
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
  std::cout << "CamStreamer::handleFailedRequest" << std::endl;
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
