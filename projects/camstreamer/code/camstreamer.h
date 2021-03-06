#ifndef CAMSTREAMER_H
#define CAMSTREAMER_H

#include "config.h"
#ifdef LICENSED
#include "license.h" // remember that Stream is secretly the license
#endif
#include "requestmanager.h"

#include "dled_controller.h"
#include "dgp_modecontroller.h"

#include "kn_asiocallbacktimer.h"
#include "kn_anydeleteontimeout.h"

#include "utl_filelogger.h"

#include "wifi_configurator.h"
#include "wp_edgeinputpin.h"

#include <memory>

class CamStreamer final : RequestManager::Owner,
  D_GP::ModeController::Owner,
  D_LED::Controller::Owner,
  P_WIFI::Configurator::Owner,
  Config::Owner
{
 public:
  CamStreamer(std::string config_file_path);
  void start();

 private:
  // RequestManager::Owner
  void handleReqNextMode(RequestManager*) override;
  void handleReqTrigger(RequestManager*) override;
  void handleReqShutdown(RequestManager*) override;
  void handleReqReboot(RequestManager*) override;
  void handleError(RequestManager*,
		   RequestManager::Error,
		   const std::string& msg) override;
  
  // D_GP::ModeController::Owner
  void handleFailedRequest(D_GP::ModeController*,
			   D_GP::ModeController::Req,
			   const std::string&) override;
  void handleInternalFailure(D_GP::ModeController*) override;
  void handleSuccessfulRequest(D_GP::ModeController*, D_GP::ModeController::Req) override;

  // D_LED::Controller::Owner
  void handleReqFailed(D_LED::Controller*,
		       D_LED::Controller::Req req,
		       const std::string& msg) override;
  void handleInternalError(D_LED::Controller*) override;

  // P_WIFI::Owner
  void handleError(P_WIFI::Configurator*,
		   P_WIFI::Configurator::Error,
		   const std::string& msg) override;

  // Config::Owner
  void handleError(Config*,
		   Config::Error,
		   const std::string& msg) override;

 private:
  enum class State
  {
    InvalidConfig,
      Connecting,
      Connected,
      InternalFailure,
      Unknown
      };

 private:
  void setupWifi();
  void restartGPController();
  void stop();

  void setState(State);
  void processState();
  void ledForPinReq();
  
 private:
  State d_state = State::Unknown;
  std::unique_ptr<RequestManager> d_req_man;
  std::unique_ptr<P_WIFI::Configurator> d_wifi_config;
  std::unique_ptr<Config> d_config;
  
  D_GP::ModeController::CtrlParams d_gpcont_params;
  std::unique_ptr<D_GP::ModeController> d_gp_controller;
  KERN::AsioCallbackTimer d_restart_gp_timer = KERN::AsioCallbackTimer("CamStreamer - reset gopro timer.");
  
  // C_BLE::Serial d_bl_connection; use for phone app messages (pairing request, gopro details, pin numbers)

  std::unique_ptr<D_LED::Controller> d_led_ctrl;

  KERN::AnyDeleteOnTimeout d_timeout_deleter;

  std::unique_ptr<UTIL::FileLogger> d_logger;

#ifdef LICENSED
  std::unique_ptr<Stream> d_license;
#endif
};

#endif
