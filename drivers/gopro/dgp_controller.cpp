#include "dgp_controller.h"

#include "dgp_factory.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

//----------------------------------------------------------------------//
GoProController::GoProController(GoProControllerOwner* o, GPCtrlParams p)
  : d_owner(o),
    d_gp(GoProFactory::createGoPro(p.model, this)),
    d_connect_name(p.name),
    d_state(new StateDisconnected)
{
  assert(o!=nullptr);
  
  // add list of possible states
  d_states[GPStateId::Disconnected] = std::unique_ptr<GPState>(new StateDisconnected);
  d_states[GPStateId::Connected] = std::unique_ptr<GPState>(new StateConnected);
  d_states[GPStateId::Photo] = std::unique_ptr<GPState>(new StatePhoto);
  d_states[GPStateId::StartStopRec] = std::unique_ptr<GPState>(new StateVideo);
  d_state = d_states[GPStateId::Connected].get();
}

//----------------------------------------------------------------------//
GoProController::~GoProController() = default;

//----------------------------------------------------------------------//
void GoProController::connect()
{
  setState(GPStateId::Disconnected);
  processCurrentState();
}

//----------------------------------------------------------------------//
void GoProController::takePhoto()
{
  setState(GPStateId::Photo);
  processCurrentState();
}

//----------------------------------------------------------------------//
void GoProController::StartStopRecording()
{
  setState(GPStateId::StartStopRec);
  processCurrentState();
}

//----------------------------------------------------------------------//
void GoProController::handleCommandFailed(GoPro*, Cmd cmd)
{
  std::cout << "handleCommandFailed: " << cmdToString(cmd) << std::endl;

  d_mode = Mode::Unknown; // cause any set mode to happen
  setState(GPStateId::Disconnected);
  d_owner->handleFailedRequest(this); // I am concerned about a command failing after a recording has started
}

//----------------------------------------------------------------------//
void GoProController::setState(GPStateId id)
{	
  d_prev_state_id = d_state_id;
  d_state_id = id;
  d_state = d_states[id].get();
}

//----------------------------------------------------------------------//
void GoProController::toggleRecording()
{
  setMode(Mode::Video);
  d_gp->setShutter(!d_is_recording);
  d_is_recording = !d_is_recording;
}

//----------------------------------------------------------------------//
void StateDisconnected::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);
	
  switch (ctrl.getPrevState())
    {
    case GPStateId::Disconnected:
    case GPStateId::Connected:
    case GPStateId::Photo:
      break;
			
    case GPStateId::StartStopRec:
      if (ctrl.d_is_recording)
	{
	  ctrl.toggleRecording(); // stop recording
	}
      break;

    default:
      assert(false);
      return;
      break;
    };
}

//----------------------------------------------------------------------//
void StateConnected::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);
  GoPro& gp = *ctrl.d_gp;
	
  switch (ctrl.getPrevState())
    {
    case GPStateId::Disconnected:
    case GPStateId::Connected:
    case GPStateId::Photo:
      gp.connectWithName(ctrl.d_connect_name);
      break;
			
    case GPStateId::StartStopRec:
      if (ctrl.d_is_recording)
	{
	  ctrl.toggleRecording(); // stop recording
	}
      gp.connectWithName(ctrl.d_connect_name);
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
    case GPStateId::Disconnected:
      gp.connectWithName(ctrl.d_connect_name);
    case GPStateId::Connected:
      ctrl.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
      break;
			
    case GPStateId::Photo:
      ctrl.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
      break;
			
    case GPStateId::StartStopRec:
      // stop recording if trying to take a pic
      // or just take a pic
      if (ctrl.d_is_recording)
	{
	  ctrl.toggleRecording();
	}
      else
	{
	  ctrl.setMode(Mode::Photo);
	  gp.setShutter(true); // take pic
	}
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
    case GPStateId::Disconnected:
      gp.connectWithName(ctrl.d_connect_name);
    case GPStateId::Connected:
    case GPStateId::Photo:
      assert(!ctrl.d_is_recording);
      ctrl.setMode(Mode::Video);
      ctrl.toggleRecording(); // start video
      break;
			
    case GPStateId::StartStopRec:
      ctrl.toggleRecording();
      break;

    default:
      assert(false);
      return;
      break;
    };
}

//----------------------------------------------------------------------//
void GoProController::processCurrentState()
{
  d_state->process(*this);
}

//----------------------------------------------------------------------//
void GoProController::setMode(Mode mode)
{
  //if (d_mode != mode)
  //{
  d_gp->setMode(mode); // always just set the mode for now to ensure the correct mode is being used
      //}
}
