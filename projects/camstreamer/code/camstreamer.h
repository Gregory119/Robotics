#ifndef CAMSTREAMER_H
#define CAMSTREAMER_H

#include "dgp_modecontroller.h"

#include "kn_asiocallbacktimer.h"
#include "kn_deleteontimeout.h"

#include <wp_pins.h>
#include <memory>

class CamStreamer final : D_GP::ModeController::Owner
{
 public:
  CamStreamer();

 private:
  void readPins();
  void processPins();

 private:
  void handleFailedRequest(ModeController*, ModeController::Req) override;
  void handleSuccessfulRequest(ModeController*, ModeController::Req) override;

 private:
  void restartGPController();
  
 private:
  std::unique_ptr<D_GP::ModeController> d_gp_controller;
  // C_BLE::Serial d_bl_connection; use for phone app messages (pairing request, gopro details, pin numbers)
  
  int d_pin_mode_num = P_WP::PinH11;
  int d_pin_trigger_num = P_WP::PinH13;
  int d_pin_connect_num = P_WP::PinH15;
  
  bool d_pin_mode_val = false;
  bool d_pin_trigger_val = false;
  bool d_pin_connect_val = false;

  ModeController::CtrlParams d_gpcont_params;;

  KERN::AsioCallbackTimer d_check_pins;
  KERN::AsioCallbackTimer d_reset_gp;
    
  KERN::DeleteOnTimeout<D_GP::ModeController> d_delete_gpcont;
};

#endif
