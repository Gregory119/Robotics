#ifndef CAMSTREAMER_H
#define CAMSTREAMER_H

#include "config.h"

#include "ledcontroller.h"
#include "dgp_modecontroller.h"

#include "kn_asiocallbacktimer.h"
#include "kn_deleteontimeout.h"
#include "wp_edgeinputpin.h"

#include <memory>

class CamStreamer final : Config::Owner,
  D_GP::ModeController::Owner,
  LedController::Owner
{
 public:
  CamStreamer(const std::string& config_file_path);
  void start();

 private:
  // Config::Owner
  void handleError(Config*, Config::Error, const std::string& msg) override;
  
  // D_GP::ModeController::Owner
  void handleFailedRequest(D_GP::ModeController*, D_GP::ModeController::Req) override;
  void handleSuccessfulRequest(D_GP::ModeController*, D_GP::ModeController::Req) override;

  // LedController::Owner
  void handleStateChange(LedController*, LedController::State) override;
  void handleError(LedController*, const std::string& msg) override;

 private:
  void restartGPController();
  void processModePinState(bool);
  void processConnectPinState(bool);
  void processTriggerPinState(bool);

  void stop();

 private:
  std::unique_ptr<Config> d_config;
  KERN::DeleteOnTimeout<Config> d_delete_config;

  D_GP::ModeController::CtrlParams d_gpcont_params;
  std::unique_ptr<D_GP::ModeController> d_gp_controller;
  KERN::DeleteOnTimeout<D_GP::ModeController> d_delete_gp_cont;
  KERN::AsioCallbackTimer d_reset_gp_timer = KERN::AsioCallbackTimer("CamStreamer - reset gopro timer.");
  
  // C_BLE::Serial d_bl_connection; use for phone app messages (pairing request, gopro details, pin numbers)

  std::unique_ptr<P_WP::EdgeInputPin> d_mode_pin;
  std::unique_ptr<P_WP::EdgeInputPin> d_trigger_pin;
  std::unique_ptr<P_WP::EdgeInputPin> d_connect_pin;

  std::unique_ptr<LedController> d_led_ctrl;
};

#endif
