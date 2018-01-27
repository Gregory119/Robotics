#ifndef CAMSTREAMER_H
#define CAMSTREAMER_H

#include "config.h"

#include "dgp_modecontroller.h"

#include "kn_asiocallbacktimer.h"
#include "kn_deleteontimeout.h"
#include "wp_edgeinputpin.h"

#include <memory>

// LED:
// basic led control class
// camstreamer specific led control class, which will own a basic led control class
// incorrect settings file (very short on and off)
// connecting (short on, long off)
// failed to connect (short on and off)
// connected (solid)
// command successful (short on and off for 1 second) => back to connected
// command failed (very short on and off for 1 second) => back to connected

class CamStreamer final : Config::Owner,
  D_GP::ModeController::Owner
{
 public:
  CamStreamer(const std::string& config_file_path);

 private:
  // Config::Owner
  void handleError(Config*, Config::Error, const std::string& msg) override;
  
  // D_GP::ModeController
  void handleFailedRequest(D_GP::ModeController*, D_GP::ModeController::Req) override;
  void handleSuccessfulRequest(D_GP::ModeController*, D_GP::ModeController::Req) override;

 private:
  void restartGPController();
  void processModePinState(bool);
  void processConnectPinState(bool);
  void processTriggerPinState(bool);

 private:
  std::unique_ptr<Config> d_config;
  
  std::unique_ptr<D_GP::ModeController> d_gp_controller;
  // C_BLE::Serial d_bl_connection; use for phone app messages (pairing request, gopro details, pin numbers)
  
  std::unique_ptr<P_WP::EdgeInputPin> d_mode_pin;
  std::unique_ptr<P_WP::EdgeInputPin> d_trigger_pin;
  std::unique_ptr<P_WP::EdgeInputPin> d_connect_pin;

  D_GP::ModeController::CtrlParams d_gpcont_params;

  KERN::AsioCallbackTimer d_reset_gp = KERN::AsioCallbackTimer("CamStreamer - reset gopro timer.");

  KERN::DeleteOnTimeout<Config> d_delete_config;
  KERN::DeleteOnTimeout<D_GP::ModeController> d_delete_gpcont;
  // hold a led controller
};

#endif
