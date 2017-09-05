#include "dgp_controller.h"

#include "dgp_factory.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

//----------------------------------------------------------------------//
GoProController::GoProController(GoProControllerOwner* o, GPCtrlParams p)
  : d_owner(o),
    d_gp(GoProFactory::createGoPro(this, p.model, p.name)),
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
void GoProController::connectReq()
{
	d_is_recording = false; // assumption
  d_reqs.emplace_back(GoProControllerCmd::Connect);
  processCurrentState();
}


//----------------------------------------------------------------------//
void GoProController::takePhotoReq()
{
  d_cmd = GoProControllerCmd::Photo;
  setState(GPStateId::Photo);
  processCurrentState();
}

//----------------------------------------------------------------------//
void GoProController::startStopRecordingReq()
{
  d_cmd = GoProControllerCmd::ToggleRecording;
  setState(GPStateId::StartStopRec);
  processCurrentState();
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
GoProControllerCmd GoProController::getLastRequest()
{
	return ctrl.d_reqs.front();
}

//----------------------------------------------------------------------//
void StateDisconnected::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);

	GoPro& gp = *ctrl.d_gp;
	GoProControllerCmd &req = ctrl.getLastRequest();
	switch (req)
    {
		case GoProControllerCmd::Unknown:
			assert(false);
			return;
			
		case GoProControllerCmd::Connect:
			gp.connect();
			return;
			
		case GoProControllerCmd::Photo:
			gp.connect();
			gp.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
			return;
			
		case GoProControllerCmd::ToggleRecording:
			//NOTE: CHANGE THIS IN FUTURE TO CHECK GOPRO STATUS TO SEE WHETHER STILL RECORDING
      //if (ctrl.d_is_recording)
			//{
	  // ctrl.toggleRecording(); // stop recording
			//}
			gp.connect();
			ctrl.toggleRecording();
      return;
    };
	assert(false);
}

//----------------------------------------------------------------------//
void StateConnected::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);

	GoPro& gp = *ctrl.d_gp;
	GoProControllerCmd &req = ctrl.getLastRequest();
	switch (req)
    {
		case GoProControllerCmd::Unknown:
			assert(false);
			return;
			
		case GoProControllerCmd::Connect:
			gp.connect();
			return;
			
		case GoProControllerCmd::Photo:
			gp.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
			return;
			
		case GoProControllerCmd::ToggleRecording:
			ctrl.toggleRecording();
      return;
    };
	assert(false);





	
  GoPro& gp = *ctrl.d_gp;
  switch (ctrl.getPrevState())
    {
    case GPStateId::Disconnected:
    case GPStateId::Connected:
    case GPStateId::Photo:
      gp.connect();
      break;
			
    case GPStateId::StartStopRec:
      if (ctrl.d_is_recording)
	{
	  ctrl.toggleRecording(); // stop recording
	}
      gp.connect();
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
      gp.connect();
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
      gp.connect();
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
  d_gp->setMode(mode);
}

//----------------------------------------------------------------------//
void GoProController::handleCommandSuccessful(GoPro*, Cmd cmd)
{
  switch (cmd)
    {
    case Cmd::SetModePhoto:
      return;

    case Cmd::SetModeVideo:
      return;

    case Cmd::Connect:
			setState(GPStateId::Connected);
			return;
			
    case Cmd::SetShutterTrigger:
    case Cmd::SetShutterStop:
    case Cmd::LiveStream:
      return;

    case Cmd::Unknown:
      assert(false);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
void GoProController::handleCommandFailed(GoPro*, Cmd cmd, GPError err)
{
	// consider processing the type of error
  std::cout << "GoProController::handleCommandFailed " << std::endl;

	setState(GPStateId::Disconnected);
  connectReq(); // try to reconnect

  // do this last because owner may request a new command, resulting in a state change and process
	// POP THE FRONT REQUEST QUEUE TO SEND WITH FAILURE NOTIFICATION
	// CLEAR THE REST OF THE QUEUED MESSAGES
	d_owner->handleFailedRequest(this, d_cmd);
}
