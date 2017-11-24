#include "camstreamer.h"

constexpr char s_mode_pin[] = "mode";
constexpr char s_trigger_pin[] = "trigger";
constexpr char s_connect_pin[] = "connect";

//----------------------------------------------------------------------//
CamStreamer::CamStreamer()
{
  // extract pin numbers from xml
  // d_pin_mode_num = ??;
  // d_pin_trigger_num = ??;
  // pin functional name
  // pin header number
  // ARE SERVO RECEIVER OUTPUT PINS NORMALLY HIGH OR LOW??? SUPPORT OTHER RC RECEIVER TECHNOLOGIES (NEED SETUP ON THE APP)??

  // Pins
  P_WP::InputPin mode_pin(P_WP::PinH11);
  mode_pin.setStateChangedCallback([this](bool state){
      processModePinState(state);
    });
  P_WP::InputPin trigger_pin(P_WP::PinH11);
  trigger_pin.setStateChangedCallback([this](bool state){
      processTriggerPinState(state);
    });
  P_WP::InputPin connect_pin(P_WP::PinH11);
  connect_pin.setStateChangedCallback([this](bool state){
      processConnectPinState(state);
    });
  d_pins.emplace(std::make_pair(s_mode_pin, std::move(mode_pin)));
  d_pins.emplace(std::make_pair(s_trigger_pin, std::move(trigger_pin)));
  d_pins.emplace(std::make_pair(s_connect_pin, std::move(connect_pin)));

  // extract gopro type from xml (app setting)
  d_gpcont_params.setType(D_GP::CamModel::Hero5).setName("CamStreamer");
  restartGPController();
  
  d_reset_gp.setCallback([this](){
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
  d_gp_controller->nextMode();
}

//----------------------------------------------------------------------//
void CamStreamer::processConnectPinState(bool state)
{
  if (state) // high edge
    {
      d_gp_controller->connect();
    }
}

//----------------------------------------------------------------------//
void CamStreamer::processTriggerPinState(bool)
{
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
