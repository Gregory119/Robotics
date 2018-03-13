#ifndef CAMSTREAMER_H
#define CAMSTREAMER_H

#include "config.h"

#include "dled_controller.h"
#include "dgp_modecontroller.h"

#include "kn_asiocallbacktimer.h"
#include "kn_anydeleteontimeout.h"
#include "wifi_configurator.h"
#include "wp_edgeinputpin.h"

#include <memory>

class CamStreamer final : Config::Owner,
  D_GP::ModeController::Owner,
  D_LED::Controller::Owner,
  P_WIFI::Configurator::Owner
{
 public:
  CamStreamer(const std::string& config_file_path);
  void start();

 private:
  // Config::Owner
  void handleError(Config*, Config::Error, const std::string& msg) override;
  
  // D_GP::ModeController::Owner
  void handleFailedRequest(D_GP::ModeController*, D_GP::ModeController::Req) override;
  void handleInternalFailure(D_GP::ModeController*) override;
  void handleSuccessfulRequest(D_GP::ModeController*, D_GP::ModeController::Req) override;

  // D_LED::Controller::Owner
  void handleFlashCycleEnd(D_LED::Controller*) override;
  void handleReqFailed(D_LED::Controller*,
		       D_LED::Controller::Req req,
		       const std::string& msg) override;
  void handleInternalError(D_LED::Controller*) override;

  // P_WIFI::Owner
  void handleError(P_WIFI::Configurator*,
		   P_WIFI::Configurator::Error,
		   const std::string& msg) override;

 private:
  enum class State
  {
    InvalidConfig,
      Connecting,
      Connected,
      CommandSuccessful,
      InternalFailure,
      Unknown
      };
  
 private:
  void setupWifi();
  void restartGPController();
  void processModePinState(bool);
  void processTriggerPinState(bool);

  void setState(State);
  
  void stop();

 private:
  std::unique_ptr<Config> d_config;
  std::unique_ptr<P_WIFI::Configurator> d_wifi_config;
  
  D_GP::ModeController::CtrlParams d_gpcont_params;
  std::unique_ptr<D_GP::ModeController> d_gp_controller;
  KERN::AsioCallbackTimer d_restart_gp_timer = KERN::AsioCallbackTimer("CamStreamer - reset gopro timer.");
  
  // C_BLE::Serial d_bl_connection; use for phone app messages (pairing request, gopro details, pin numbers)

  std::unique_ptr<P_WP::EdgeInputPin> d_mode_pin;
  std::unique_ptr<P_WP::EdgeInputPin> d_trigger_pin;

  std::unique_ptr<D_LED::Controller> d_led_ctrl;

  KERN::AnyDeleteOnTimeout d_timeout_deleter;
};

#endif
