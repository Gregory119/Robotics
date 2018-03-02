#include "camstreamer.h"

#include <cstdlib>
#include <fstream>
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

  // Set up wifi
  setupWifi();
  
  // Pins
  d_mode_pin.reset(new P_WP::EdgeInputPin(d_config->getPinNum(Config::PinId::Mode),
					  d_config->getPinPullMode(Config::PinId::Mode),
					  P_WP::EdgeInputPin::EdgeType::Both));
  d_trigger_pin.reset(new P_WP::EdgeInputPin(d_config->getPinNum(Config::PinId::Trigger),
					     d_config->getPinPullMode(Config::PinId::Trigger),
					     P_WP::EdgeInputPin::EdgeType::Both));
  d_mode_pin->setTriggerCallback([this](bool state){
      processModePinState(state);
    });
  d_trigger_pin->setTriggerCallback([this](bool state){
      processTriggerPinState(state);
    });  
  d_mode_pin->setUpdateInterval(s_pin_update_time);
  d_trigger_pin->setUpdateInterval(s_pin_update_time);

  d_gpcont_params.setType(D_GP::CamModel::Hero5).setName("CamStreamer");
  restartGPController();
  d_led_ctrl->setState(LedController::State::Connecting);
  
  d_restart_gp_timer.setTimeoutCallback([this](){
      restartGPController();
    });
}

//----------------------------------------------------------------------//
void CamStreamer::setupWifi()
{
  // open the file and extract the current settings
  d_wifi_config.reset(new P_WIFI::Configurator(this,"/etc/wpa_supplicant/wpa_supplicant.conf"));
  d_wifi_config->parseFile();
  if (d_wifi_config->hasError())
    {
      assert(false);
      return;
    }

  // only set wifi config and restart if the new config is different
  const std::string& new_ssid = d_config->getWifiSsid();
  const std::string& new_pw = d_config->getWifiPassword();

  if (new_ssid == d_wifi_config->getSsid() &&
      new_pw == d_wifi_config->getPassword())
    {
      return;
    }
  
  d_wifi_config->setSsidWithQuotes(new_ssid);
  d_wifi_config->setPasswordWithQuotes(new_pw);
  
  // Restart the wifi
  // These commands are currently synchronous
  // If they need the sudo password then add the following in front: echo raspberry | sudo -S 
  int resp = system("wpa_cli -i wlan0 reconfigure");
  std::cout << "The system call of 'wpa_cli -i wlan0 reconfigure' returned with the value "
  	    << resp << "." << std::endl;
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(Config*,
			      Config::Error e,
			      const std::string& msg)
{
  std::cerr << msg << std::endl;
  d_led_ctrl->setState(LedController::State::InvalidConfig);
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
  d_timeout_deleter.deletePtr(d_gp_controller);
  d_restart_gp_timer.singleShot(std::chrono::seconds(5));
  
  d_led_ctrl->setState(LedController::State::Connecting);
}

//----------------------------------------------------------------------//
void CamStreamer::handleSuccessfulRequest(D_GP::ModeController*,
					  D_GP::ModeController::Req)
{
  d_led_ctrl->setState(LedController::State::CommandSuccessful);
}

//----------------------------------------------------------------------//
void CamStreamer::stop()
{
  d_wifi_config->setOwner(nullptr); // disable any other callbacks
  d_timeout_deleter.deletePtr(d_wifi_config);   
  
  d_config->setOwner(nullptr);
  d_timeout_deleter.deletePtr(d_config);   
  
  d_gp_controller->setOwner(nullptr);
  d_timeout_deleter.deletePtr(d_gp_controller); 
  d_restart_gp_timer.disable();
}

//----------------------------------------------------------------------//
void CamStreamer::handleStateChange(LedController* ctrl,
				    LedController::State s)
{
  switch (s)
    {
    case LedController::State::InvalidConfig:
      {
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 5;
	adv_set.flash_count = 2;
	adv_set.start_end_between_cycle_delay = std::chrono::milliseconds(500);
	adv_set.cycle = D_LED::Driver::FlashCycle::Continuous;
      
	ctrl->flashAdvanced(adv_set);
      }
      return;

    case LedController::State::Connecting:
      {
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 1;
	adv_set.flash_count = 1;
	adv_set.start_end_between_cycle_delay = std::chrono::milliseconds(0);
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
	adv_set.flashes_per_sec = 5;
	adv_set.flash_count = 1;
	adv_set.start_end_between_cycle_delay = std::chrono::milliseconds(100);
	adv_set.cycle = D_LED::Driver::FlashCycle::OnceOff;
      
	ctrl->flashAdvanced(adv_set);
      }
      return;
      
    case LedController::State::InternalFailure:
      {
	assert(false);
	std::cerr << "CamStreamer::handleStateChange - LedController internal failure." << std::endl;
	stop();
	
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 5;
	adv_set.flash_count = 3;
	adv_set.start_end_between_cycle_delay = std::chrono::milliseconds(500);
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
void CamStreamer::handleError(LedController*, const std::string& msg)
{
  std::cerr << "CamStreamer::handleError " << msg << std::endl;
  d_led_ctrl->setState(LedController::State::InternalFailure);
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(P_WIFI::Configurator*,
			      P_WIFI::Configurator::Error e,
			      const std::string& msg)
{
  std::cerr << msg << std::endl;
  // no errors here show whether the configuration values are not what they should be
  d_led_ctrl->setState(LedController::State::InternalFailure);
}
