#include "camstreamer.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

//----------------------------------------------------------------------//
CamStreamer::CamStreamer(std::string config_file_path)
  : d_req_man(new RequestManager(this, config_file_path)),
    d_wifi_config(new P_WIFI::Configurator(this,"/etc/wpa_supplicant/wpa_supplicant.conf")),
    d_config(new Config(this, std::move(config_file_path))),
    d_led_ctrl(new D_LED::Controller(this))
{
  UTIL::FileLogger::Params params;
  params.file_name = "GoPie";
    
  d_logger.reset(new UTIL::FileLogger(params));
}

//----------------------------------------------------------------------//
void CamStreamer::start()
{
  if (d_logger->hasError())
    {
      assert(false);
      return;
    }
  
  d_config->parseFile();
  if (d_config->hasError())
    {
      return;
    }

  // LOG: the configuration being used
  
  setupWifi();

  d_gpcont_params.setType(D_GP::CamModel::Hero5).setName("CamStreamer");
  setState(State::Connecting);
  restartGPController();
  
  d_restart_gp_timer.setTimeoutCallback([this](){
      restartGPController();
    });
  d_req_man->start();
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqNextMode(RequestManager*)
{
  d_logger->log(INFO, "Detected user request to change to the next mode.");
  if (d_gp_controller == nullptr)
    {
      return;
    }
  ledForPinReq();
  d_gp_controller->nextMode();
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqTrigger(RequestManager*)
{
  d_logger->log(INFO, "Detected user request to trigger.");
  if (d_gp_controller == nullptr)
    {
      return;
    }
  ledForPinReq();
  d_gp_controller->trigger();
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqShutdown(RequestManager*)
{
  ledForPinReq();
  d_logger->log(INFO, "Detected user request to shutdown.");
  stop();
  int resp = system("shutdown now");
  std::ostringstream stream("The system call of 'shutdown now' returned with the value ",
			    std::ios_base::app);
  stream << resp << ".";
  d_logger->log(INFO, stream.str());
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqReboot(RequestManager*)
{
  ledForPinReq();
  d_logger->log(INFO, "Detected user request to reboot.");
  stop();
  int resp = system("shutdown -r now");
  std::ostringstream stream("The system call of 'shutdown now' returned with the value ",
			    std::ios_base::app);
  stream << resp << ".";
  d_logger->log(INFO, stream.str());
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(RequestManager*,
			      RequestManager::Error e,
			      const std::string& msg)
{
  d_logger->log(ERROR, "The request manager failed with the following message:");
  d_logger->log(ERROR, msg);
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
				      D_GP::ModeController::Req req,
				      const std::string& msg)
{
  std::ostringstream stream("The GoPro request ",
			    std::ios_base::app);
  stream << D_GP::ModeController::getReqStr(req) << " failed. "
	 << "Please check that the GoPro is on, its WiFi is active and that the WiFi SSID and password in the user configuration file is correct."
	 << "The following failure message was received:\n" << msg;
  d_logger->log(ERROR, stream.str());

  d_gp_controller->setOwner(nullptr);
  d_timeout_deleter.deletePtr(d_gp_controller);
  d_restart_gp_timer.singleShot(std::chrono::seconds(10));
  
  setState(State::Connecting);
}

//----------------------------------------------------------------------//
void CamStreamer::handleInternalFailure(D_GP::ModeController*)
{
  // this should not happen
  d_logger->log(CRIT, "The GoPro controller had an internal failure.");
  setState(State::InternalFailure);
}

//----------------------------------------------------------------------//
void CamStreamer::handleSuccessfulRequest(D_GP::ModeController*,
					  D_GP::ModeController::Req req)
{
  std::ostringstream stream("The GoPro request ", std::ios_base::app);
  stream << D_GP::ModeController::getReqStr(req) << " was successful. ";
  d_logger->log(INFO, stream.str());
  setState(State::Connected);
}

//----------------------------------------------------------------------//
void CamStreamer::handleReqFailed(D_LED::Controller*,
				  D_LED::Controller::Req req,
				  const std::string& msg)
{
  std::string new_msg = "The LED controller failed with the following message:\n";
  new_msg += msg;
  d_logger->log(ERROR, new_msg);
  
  assert(false);
  d_led_ctrl->setOwner(nullptr); // disable any other callbacks
  d_timeout_deleter.deletePtr(d_led_ctrl);
  stop();
}

//----------------------------------------------------------------------//
void CamStreamer::handleInternalError(D_LED::Controller*)
{
  // this should not happen
  d_logger->log(CRIT, "The LED controller had an internal failure.");
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
  std::cerr << "CamStreamer::handleError(P_WIFI::Configurator*..) - The Wifi configurator failed with the following error message:\n"
	    << msg << "\n" << std::endl;
  d_logger->log(CRIT, "The LED controller had an internal failure.");
  // no errors here show whether the configuration values are not what they should be
  setState(State::InternalFailure);
}

//----------------------------------------------------------------------//
void CamStreamer::handleError(Config*,
			      Config::Error e,
			      const std::string& msg)
{
  std::string new_msg = "The user configuration failed with the following message:\n";
  new_msg += msg;
  d_logger->log(ERROR, new_msg);
  setState(State::InvalidConfig);
}

//----------------------------------------------------------------------//
void CamStreamer::setupWifi()
{
  d_wifi_config->parseFile();
  if (d_wifi_config->hasError())
    {
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
  std::ostringstream stream("The system call of 'wpa_cli -i wlan0 reconfigure' returned with the value ",
			    std::ios_base::app);
  stream << resp << ".";
  d_logger->log(INFO, stream.str());
  setState(State::InvalidConfig);
}

//----------------------------------------------------------------------//
void CamStreamer::restartGPController()
{
  d_gp_controller.reset(new D_GP::ModeController(this, d_gpcont_params));
  d_gp_controller->connect();
  d_gp_controller->startStream();
}

//----------------------------------------------------------------------//
void CamStreamer::stop()
{
  d_config->setOwner(nullptr); // disable any other callbacks
  // no need to delete
  
  d_wifi_config->setOwner(nullptr);
  // no need to delete
  
  d_req_man->setOwner(nullptr);

  if (d_gp_controller != nullptr)
    {
      d_gp_controller->setOwner(nullptr);
    }
  d_restart_gp_timer.disableIfEnabled();
}

//----------------------------------------------------------------------//
void CamStreamer::setState(State s)
{
  d_state = s;
  processState();
}

//----------------------------------------------------------------------//
void CamStreamer::processState()
{
  switch (d_state)
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
      
    case State::InternalFailure:
      {
	assert(false);
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
      std::cerr << "CamStreamer::processState - Camstreamer::State is unknown." << "\n" << std::endl;
      d_logger->log(CRIT, "The camera manager had an internal error because its state is unknown.");
      stop();
      return;
    }
  
  assert(false);
  std::ostringstream stream("The camera manager had an internal error because its state is invalid. Its value is ",
			    std::ios_base::app);
  stream << static_cast<int>(d_state) << ".";
  d_logger->log(CRIT, stream.str());
  setState(State::InternalFailure);
}

//----------------------------------------------------------------------//
void CamStreamer::ledForPinReq()
{
  D_LED::Driver::AdvancedSettings adv_set;
  adv_set.flashes_per_sec = 5;
  adv_set.flash_count = 1;
  adv_set.start_end_between_cycle_delay = std::chrono::milliseconds(100);
  adv_set.cycle = D_LED::Driver::FlashCycle::OnceOff;
      
  d_led_ctrl->flashAdvanced(adv_set);

  processState();
}
