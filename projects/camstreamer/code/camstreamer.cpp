#include "camstreamer.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

//----------------------------------------------------------------------//
CamStreamer::CamStreamer(std::string config_file_path)
  : d_req_man(new RequestManager(this, config_file_path)),
    d_config(new Config(this, std::move(config_file_path))),
    d_led_ctrl(new D_LED::Controller(this))
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

  setupWifi();

  d_gpcont_params.setType(D_GP::CamModel::Hero5).setName("CamStreamer");
  restartGPController();
  setState(State::Connecting);
  
  d_restart_gp_timer.setTimeoutCallback([this](){
      restartGPController();
    });

  d_req_man->start(); // enable requests once everything is constructed and set
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqNextMode(RequestManager*)
{
  std::cout << "CamStreamer::handleReqNextMode()" << std::endl;
  if (d_gp_controller == nullptr)
    {
      return;
    }
  d_gp_controller->nextMode();
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqTrigger(RequestManager*)
{
  std::cout << "CamStreamer::handleReqTrigger()" << std::endl;
  if (d_gp_controller == nullptr)
    {
      return;
    }
  d_gp_controller->trigger();
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqShutdown(RequestManager*)
{
  std::cout << "CamStreamer::handleReqShutdown()" << std::endl;
  stop();
  int resp = system("shutdown now");
  std::cout << "The system call of 'shutdown now' returned with the value "
  	    << resp << "." << std::endl;
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqReboot(RequestManager*)
{
  std::cout << "CamStreamer::handleReqShutdown()" << std::endl;
  stop();
  int resp = system("shutdown -r now");
  std::cout << "The system call of 'shutdown now' returned with the value "
  	    << resp << "." << std::endl;
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(RequestManager*,
			      RequestManager::Error e,
			      const std::string& msg)
{
  std::cerr << msg << std::endl;
  // any config related errors here should be picked up first by the local config
  if (e == RequestManager::Error::Config)
    {
      setState(State::InvalidConfig);
      return;
    }
  setState(State::InternalFailure);
}

//----------------------------------------------------------------------//
void CamStreamer::handleFailedRequest(D_GP::ModeController* ctrl,
				      D_GP::ModeController::Req req)
{
  std::cerr << "CamStreamer::handleFailedRequest" << std::endl;

  d_gp_controller->setOwner(nullptr);
  d_timeout_deleter.deletePtr(d_gp_controller);
  d_restart_gp_timer.singleShot(std::chrono::seconds(5));
  
  setState(State::Connecting);
}

//----------------------------------------------------------------------//
void CamStreamer::handleInternalFailure(D_GP::ModeController*)
{
  std::cerr << "CamStreamer::handleInternalFailure" << std::endl;
  setState(State::InternalFailure);
}

//----------------------------------------------------------------------//
void CamStreamer::handleSuccessfulRequest(D_GP::ModeController*,
					  D_GP::ModeController::Req)
{
  // the led controller will buffer the commands
  setState(State::CommandSuccessful);
}

//----------------------------------------------------------------------//
void CamStreamer::handleFlashCycleEnd(D_LED::Controller* ctrl)
{
  std::cout << "CamStreamer::handleFlashCycleEnd" << std::endl;
  setState(State::Connected);
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqFailed(D_LED::Controller*,
				  D_LED::Controller::Req req,
				  const std::string& msg)
{
  std::cerr << "CamStreamer::handleReqFailed(D_LED::Controller*,..)" << msg << std::endl;
  assert(false);
  d_led_ctrl->setOwner(nullptr); // disable any other callbacks
  d_timeout_deleter.deletePtr(d_led_ctrl);
  stop();
}

//----------------------------------------------------------------------//
void CamStreamer::handleInternalError(D_LED::Controller*)
{
  std::cerr << "CamStreamer::handleInternalError(D_LED::Controller*)" << std::endl;
  assert(false);
  d_led_ctrl->setOwner(nullptr); // disable any other callbacks
  d_timeout_deleter.deletePtr(d_led_ctrl);
  stop();
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(P_WIFI::Configurator*,
			      P_WIFI::Configurator::Error e,
			      const std::string& msg)
{
  std::cerr << msg << std::endl;
  // no errors here show whether the configuration values are not what they should be
  setState(State::InternalFailure);
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(Config*,
			      Config::Error e,
			      const std::string& msg)
{
  std::cerr << msg << std::endl;
  setState(State::InvalidConfig);
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
  // LOG!!!!
  std::cout << "The system call of 'wpa_cli -i wlan0 reconfigure' returned with the value "
  	    << resp << "." << std::endl;
}

//----------------------------------------------------------------------//
void CamStreamer::restartGPController()
{
  d_gp_controller.reset(new D_GP::ModeController(this, d_gpcont_params));
  d_gp_controller->connect();
  d_gp_controller->startStream();
}

//----------------------------------------------------------------------//
void CamStreamer::setState(State s)
{
  switch (s)
    {
    case State::InvalidConfig:
      {
	stop();
	
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 5;
	adv_set.flash_count = 2;
	adv_set.start_end_between_cycle_delay = std::chrono::milliseconds(500);
	adv_set.cycle = D_LED::Driver::FlashCycle::Continuous;
      
	d_led_ctrl->flashAdvanced(adv_set);
      }
      return;

    case State::Connecting:
      {
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 1;
	adv_set.flash_count = 1;
	adv_set.start_end_between_cycle_delay = std::chrono::milliseconds(0);
	adv_set.cycle = D_LED::Driver::FlashCycle::Continuous;
      
	d_led_ctrl->flashAdvanced(adv_set);
      }
      return;
      
    case State::Connected:
      d_led_ctrl->turnOn();
      return;
      
    case State::CommandSuccessful:
      {
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 5;
	adv_set.flash_count = 1;
	adv_set.start_end_between_cycle_delay = std::chrono::milliseconds(100);
	adv_set.cycle = D_LED::Driver::FlashCycle::OnceOff;
      
	d_led_ctrl->flashAdvanced(adv_set);
      }
      return;
      
    case State::InternalFailure:
      {
	assert(false);
	std::cerr << "CamStreamer::setState - internal failure." << std::endl;
	stop();
	
	D_LED::Driver::AdvancedSettings adv_set;
	adv_set.flashes_per_sec = 5;
	adv_set.flash_count = 3;
	adv_set.start_end_between_cycle_delay = std::chrono::milliseconds(500);
	adv_set.cycle = D_LED::Driver::FlashCycle::Continuous;
      
	d_led_ctrl->flashAdvanced(adv_set);
      }
      return;
      
    case State::Unknown:
      assert(false);
      std::cerr << "CamStreamer::setState - Camstreamer::State is unknown." << std::endl;
      stop();
      return;
    }
  
  assert(false);
  std::cerr << "CamStreamer::setState - Camstreamer::State is invalid." << std::endl;
  setState(State::InternalFailure);
}

//----------------------------------------------------------------------//
void CamStreamer::stop()
{
  d_config->setOwner(nullptr); // disable any other callbacks
  d_timeout_deleter.deletePtr(d_config);
  
  d_wifi_config->setOwner(nullptr);
  d_timeout_deleter.deletePtr(d_wifi_config);   
  
  d_req_man->setOwner(nullptr);
  d_timeout_deleter.deletePtr(d_req_man);   
  
  d_gp_controller->setOwner(nullptr);
  d_timeout_deleter.deletePtr(d_gp_controller); 
  d_restart_gp_timer.disable();
}
