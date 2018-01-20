#ifndef CAMSTREAMER_H
#define CAMSTREAMER_H

#include "dgp_modecontroller.h"

#include "kn_asiocallbacktimer.h"
#include "kn_deleteontimeout.h"
#include "wp_edgeinputpin.h"

#include <memory>

class CamStreamer final : D_GP::ModeController::Owner
{
 public:
  CamStreamer();

 private:
  void handleFailedRequest(D_GP::ModeController*, D_GP::ModeController::Req) override;
  void handleSuccessfulRequest(D_GP::ModeController*, D_GP::ModeController::Req) override;

 private:
  void restartGPController();
  void processModePinState(bool);
  void processConnectPinState(bool);
  void processTriggerPinState(bool);

 private:
  std::unique_ptr<D_GP::ModeController> d_gp_controller;
  // C_BLE::Serial d_bl_connection; use for phone app messages (pairing request, gopro details, pin numbers)
  
  P_WP::EdgeInputPin d_mode_pin;
  P_WP::EdgeInputPin d_trigger_pin;
  P_WP::EdgeInputPin d_connect_pin;

  D_GP::ModeController::CtrlParams d_gpcont_params;

  KERN::AsioCallbackTimer d_reset_gp = KERN::AsioCallbackTimer("CamStreamer - reset gopro timer.");
    
  KERN::DeleteOnTimeout<D_GP::ModeController> d_delete_gpcont;
  // hold a led controller
};

#endif
