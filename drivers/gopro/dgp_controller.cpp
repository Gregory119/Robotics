#include "dgp_controller.h"

#include "dgp_factory.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

//----------------------------------------------------------------------//
GoProController::GoProController(GoProControllerOwner* o, GPCtrlParams p)
  : d_owner(o),
    d_gp(GoProFactory::createGoPro(p.type, this)),
    d_connect_name(p.name),
    d_state(new StateConnect)
{
  assert(o!=nullptr);
  
  // add list of possible states
  d_states[GPStateId::Connect] = std::unique_ptr<GPState>(new StateConnect);
  d_states[GPStateId::Photo] = std::unique_ptr<GPState>(new StatePhoto);
  d_states[GPStateId::StartStopRec] = std::unique_ptr<GPState>(new StateVideo);
  d_state = d_states[GPStateId::Connect].get();
}

//----------------------------------------------------------------------//
GoProController::~GoProController() = default;

//----------------------------------------------------------------------//
void GoProController::connect()
{
  setState(GPStateId::Connect);
}

//----------------------------------------------------------------------//
void GoProController::takePhoto()
{
  setState(GPStateId::Photo);
}

//----------------------------------------------------------------------//
void GoProController::StartStopRecording()
{
  setState(GPStateId::StartStopRec);
}

//----------------------------------------------------------------------//
void GoProController::handleFailedCommand(GoPro*, Cmd cmd)
{
  std::cout << "handleFailedCommand: " << cmdToString(cmd) << std::endl;

  d_is_recording = false;
  setState(GPStateId::Connect);
  d_owner->handleFailedRequest(this);
}

//----------------------------------------------------------------------//
void GoProController::setState(GPStateId id)
{	
  d_prev_state_id = d_state_id;
  d_state_id = id;
  d_state = d_states[id].get();
  d_state->process(*this);
}

//----------------------------------------------------------------------//
void StateConnect::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);
  GoPro& gp = *ctrl.d_gp;
	
  switch (ctrl.getPrevState())
    {
    case GPStateId::Connect:
      gp.connectWithName(ctrl.d_connect_name);
      break;
			
    case GPStateId::Photo:
      gp.connectWithName(ctrl.d_connect_name);
      break;
			
    case GPStateId::StartStopRec:
      gp.connectWithName(ctrl.d_connect_name);
      if (ctrl.d_is_recording)
	{
	  gp.setShutter(true); // stop recording
	  ctrl.d_is_recording = false;
	}
      break;

    default:
      assert(false);
      return;
      break;
    };
}

//----------------------------------------------------------------------//
void StatePhoto::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);
  GoPro& gp = *ctrl.d_gp;
	
  switch (ctrl.getPrevState())
    {
    case GPStateId::Connect:
      gp.connectWithName(ctrl.d_connect_name);
      gp.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
      break;
			
    case GPStateId::Photo:
      gp.setShutter(true); // take pic
      break;
			
    case GPStateId::StartStopRec:
      if (ctrl.d_is_recording)
	{
	  gp.setShutter(true); // stop recording
	  ctrl.d_is_recording = false;
	}
      gp.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
      break;

    default:
      assert(false);
      return;
      break;
    };
}

//----------------------------------------------------------------------//
void StateVideo::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);
  GoPro& gp = *ctrl.d_gp;
	
  switch (ctrl.getPrevState())
    {
    case GPStateId::Connect:
      assert(!ctrl.d_is_recording);
      gp.connectWithName(ctrl.d_connect_name);
      gp.setMode(Mode::Video);
      gp.setShutter(true); // start video
      break;
			
    case GPStateId::Photo:
      assert(!ctrl.d_is_recording);
      gp.setMode(Mode::Video);
      gp.setShutter(true); // start video
      break;
			
    case GPStateId::StartStopRec:
      ctrl.d_is_recording = !ctrl.d_is_recording;
      gp.setShutter(true); // start/stop video
      break;

    default:
      assert(false);
      return;
      break;
    };
}
