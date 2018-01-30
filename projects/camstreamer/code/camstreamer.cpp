#include "camstreamer.h"

#include <iostream>

static const std::chrono::milliseconds s_pin_update_time =
  std::chrono::milliseconds(25);

//----------------------------------------------------------------------//
CamStreamer::CamStreamer(const std::string& config_file_path)
  : d_config(new Config(this, config_file_path)),
    d_led_ctrl(new LedController(this))
{}

//----------------------------------------------------------------------//
void CamStreamer::start()
{
  d_config->parseFile();
  if (d_config->hasError())
    {
      // Logging will already have happened in handleError
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

  d_gpcont_params.setType(D_GP::CamModel::Hero5).setName("CamStreamer");
  restartGPController();
  d_led_ctrl->setState(LedController::State::Connecting);
  
  d_reset_gp_timer.setTimeoutCallback([this](){
      restartGPController();
    });
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(Config*, Config::Error e, const std::string& msg)
{
  std::cerr << msg << std::endl;
  d_led_ctrl->setState(LedController::State::InternalFailure);
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

  d_gp_controller->setOwner(nullptr);
  d_delete_gp_cont.deletePtr(d_gp_controller);
  d_reset_gp_timer.singleShot(std::chrono::seconds(5));
  
  d_led_ctrl->setState(LedController::State::Connecting);
}

//----------------------------------------------------------------------//
void CamStreamer::handleSuccessfulRequest(D_GP::ModeController*,
					  D_GP::ModeController::Req)
{
  d_led_ctrl->setState(LedController::State::CommandSuccessful);
}

//----------------------------------------------------------------------//
void CamStreamer::handleOnceOffFlashCycleEnd(LedController* ctrl,
					     LedController::State state)
{
  switch (state)
    {
    case LedController::State::InvalidConfig:
    case LedController::State::Connecting:
    case LedController::State::Connected:
    case LedController::State::InternalFailure:
    case LedController::State::Unknown:
      assert(false);
      return;

    case LedController::State::CommandSuccessful:
      ctrl->setState(LedController::State::Connected);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
void CamStreamer::stop()
{
  d_config->setOwner(nullptr); // disable other callbacks
  d_delete_config.deletePtr(d_config); 
  
  // disable any other callbacks
  d_gp_controller->setOwner(nullptr);
  // delete on timeout 
  d_delete_gp_cont.deletePtr(d_gp_controller); 
  d_reset_gp_timer.disable();
}

//----------------------------------------------------------------------//
void CamStreamer::handleStateChange(LedController* ctrl,
				    LedController::State s)
{
  switch (s)
    {
    case LedController::State::InvalidConfig:
      ctrl->flashPerSec(10);
      return;
      
    case LedController::State::Connecting:
      {
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 10;
	adv_set.flash_count = 1;
	adv_set.start_delay = std::chrono::milliseconds(500);
	adv_set.cycle = D_LED::Driver::FlashCycle::Continuous;
      
	ctrl->flashAdvanced(adv_set);
      }
      return;
      
    case LedController::State::Connected:
      ctrl->turnOn();
      return;
      
    case LedController::State::CommandSuccessful:
      {
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 10;
	adv_set.flash_count = 2;
	adv_set.start_delay = std::chrono::milliseconds(300);
	adv_set.cycle = D_LED::Driver::FlashCycle::Continuous;
      
	ctrl->flashAdvanced(adv_set);
      }
      return;
      
    case LedController::State::InternalFailure:
      {
	assert(false);
	std::cerr << "CamStreamer::handleStateChange - LedController internal failure." << std::endl;
	stop();
	
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 10;
	adv_set.flash_count = 3;
	adv_set.start_delay = std::chrono::milliseconds(500);
	adv_set.cycle = D_LED::Driver::FlashCycle::Continuous;
      
	ctrl->flashAdvanced(adv_set);
      }
      return;
      
    case LedController::State::Unknown:
      assert(false);
      std::cerr << "CamStreamer::handleStateChange - LedController::State is unknown." << std::endl;
      stop();
      return;
    }
  
  assert(false);
  std::cerr << "CamStreamer::handleStateChange - LedController::State is invalid." << std::endl;
  d_led_ctrl->setState(LedController::State::InternalFailure);
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(LedController*, const std::string& msg)
{
  std::cerr << "CamStreamer::handleError " << msg << std::endl;
  d_led_ctrl->setState(LedController::State::InternalFailure);
}
