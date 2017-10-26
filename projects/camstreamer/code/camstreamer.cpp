#include "camstreamer.h"

#include <wp_pins.h>
#include <wiringPi.h>

//----------------------------------------------------------------------//
CamStreamer::CamStreamer()
{
  // extract pin numbers from xml
  // pin functional name
  // pin header number
  // ARE SERVO RECEIVER OUTPUT PINS NORMALLY HIGH OR LOW??? SUPPORT OTHER RC RECEIVER TECHNOLOGIES (NEED SETUP ON THE APP)??

  // extract gopro type from xml (app setting)
  CtrlParams params.setType(D_GP::CamModel::Hero5).setName("CamStreamer");
  d_gp_controller.reset(new D_GP::ModeController(this,params));
  d_gp_controller->startStream();

  pinMode(P_WP::PinH11, INPUT);
  pullUpDnControl(P_WP::PinH11, PUD_DOWN); // pulled down for now
  pinMode(P_WP::PinH13, INPUT);
  pullUpDnControl(P_WP::PinH13, PUD_DOWN);

  readPins();
    
  d_check_pins.setCallback([this](){
      readPins();
      processPins();
    });
  d_check_pins.restart(std::chrono::milliseconds(30)); // do not want multiple triggers from a switch bounce
}

//----------------------------------------------------------------------//
void CamStreamer::readPins()
{
  d_pin_mode.current = digitalRead(P_WP::PinH11);
  d_pin_trigger.current = digitalRead(P_WP::PinH13);
}

//----------------------------------------------------------------------//
void CamStreamer::processPins()
{
  if (d_pin_mode.current != d_pin_mode.previous)
    {
      d_pin_mode.current = d_pin_mode.previous;
      d_gp_controller->nextMode();
    }

  if (d_pin_trigger.current != d_pin_trigger.previous)
    {
      d_pin_trigger.current = d_pin_trigger.previous;
      d_gp_controller->trigger();
    }
}

//----------------------------------------------------------------------//
void CamStreamer::handleFailedRequest(ModeController* ctrl,
				      ModeController::Req req)
{
  // disable any other callbacks
  ctrl.setOwner(nullptr);
  // delete on timeout
  // d_delete_timer.delete(d_gp_controller.release());
}

//----------------------------------------------------------------------//
void CamStreamer::handleSuccessfulRequest(ModeController*,
					  ModeController::Req)
{
  // YAY!!
}
