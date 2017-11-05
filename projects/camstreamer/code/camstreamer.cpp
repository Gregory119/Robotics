#include "camstreamer.h"

#include <wiringPi.h>

//----------------------------------------------------------------------//
CamStreamer::CamStreamer()
{
  // extract pin numbers from xml
  // d_pin_mode_num = ??;
  // d_pin_trigger_num = ??;
  // pin functional name
  // pin header number
  // ARE SERVO RECEIVER OUTPUT PINS NORMALLY HIGH OR LOW??? SUPPORT OTHER RC RECEIVER TECHNOLOGIES (NEED SETUP ON THE APP)??

  // extract gopro type from xml (app setting)
  d_gpcont_params.setType(D_GP::CamModel::Hero5).setName("CamStreamer");
  restartGPController();

  // initialize pins
  pinMode(d_pin_mode_num, INPUT);
  pullUpDnControl(d_pin_mode_num, PUD_DOWN); // pulled down for now
  pinMode(d_pin_trigger_num, INPUT);
  pullUpDnControl(d_pin_trigger_num, PUD_DOWN);
  pinMode(d_pin_connect_num, INPUT);
  pullUpDnControl(d_pin_connect_num, PUD_DOWN);

  // read pins
  d_pin_mode_val = digitalRead(d_pin_mode_num);
  d_pin_trigger_val = digitalRead(d_pin_trigger_num);
  d_pin_connect_val = digitalRead(d_pin_connect_num);
    
  // timers
  d_check_pins.setCallback([this](){
      processPins();
    });
  d_check_pins.restart(std::chrono::milliseconds(30)); // do not want multiple triggers from a switch bounce
  
  d_reset_gp.setCallback([this](){
      restartGPController();
    });
}

//----------------------------------------------------------------------//
void CamStreamer::restartGPController()
{
  d_gp_controller.reset(new D_GP::ModeController(this,d_gpcont_params));
  d_gp_controller->connect();
  d_gp_controller->startStream();
}

//----------------------------------------------------------------------//
void CamStreamer::processPins()
{
  bool pin_mode_val = digitalRead(d_pin_mode_num);
  bool pin_trigger_val = digitalRead(d_pin_trigger_num);
  bool pin_connect_val = digitalRead(d_pin_connect_num);
  if (d_pin_mode_val != pin_mode_val)
    {
      d_pin_mode_val = pin_mode_val;
      d_gp_controller->nextMode();
    }

  if (d_pin_trigger_val != pin_trigger_val)
    {
      d_pin_trigger_val = pin_trigger_val;
      d_gp_controller->trigger();
    }

  // connect on a low to high edge pin state
  if (d_pin_connect_val != pin_connect_val && d_pin_connect_val == true)
    {
      d_pin_connect_val = pin_connect_val;
      d_gp_controller->connect();
    }
}

//----------------------------------------------------------------------//
void CamStreamer::handleFailedRequest(D_GP::ModeController* ctrl,
				      D_GP::ModeController::Req req)
{
  std::cout << "CamStreamer::handleFailedRequest" << std::endl;
  // disable any other callbacks
  ctrl->setOwner(nullptr);
  // delete on timeout 
  d_delete_gpcont.deletePtr(d_gp_controller);
  d_reset_gp.singleShot(std::chrono::seconds(5));
  
  // led => display not connected
}

//----------------------------------------------------------------------//
void CamStreamer::handleSuccessfulRequest(D_GP::ModeController*,
					  D_GP::ModeController::Req)
{
  // led => display connected
}
