#ifndef CAMSTREAMER_H
#define CAMSTREAMER_H

#include "kn_asiocallbacktimer.h"
#include "dgp_modecontroller.h"

#include <memory>

class CamStreamer final : D_GP::ModeController::Owner
{
 public:
  CamStreamer();

 private:
  void readPins();
  void processPins();

 private:
  struct PinState
  {
    bool current = false;
    bool previous = false;
  };

 private:
  void handleFailedRequest(ModeController*, ModeController::Req) override;
  void handleSuccessfulRequest(ModeController*, ModeController::Req) override;
  
 private:
  std::unique_ptr<D_GP::ModeController> d_gp_controller;
  // C_BLE::Serial d_bl_connection; use for phone app messages (pairing request, gopro details, pin numbers)
  
  KERN::AsioCallbackTimer d_check_pins;
  
  PinState d_pin_mode;
  PinState d_pin_trigger;

  // KERN::DeleteTimer<D_GP::ModeController> d_delete_timer;
};

#endif
