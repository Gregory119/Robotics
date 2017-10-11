#include "dgp_fastcontroller.h"

#include "dgp_factory.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

//----------------------------------------------------------------------//
FastController::FastController(Owner* o,
			       const CtrlParams& p)
  : d_owner(o),
    d_gp(GoProFactory::createGoPro(this, p.model, p.name))
{
  assert(o!=nullptr);
  
  // add list of possible states
  d_states[StateId::Disconnected] = std::unique_ptr<StateInterface>(new StateDisconnected);
  d_states[StateId::Connected] = std::unique_ptr<StateInterface>(new StateConnected);
  d_states[StateId::Photo] = std::unique_ptr<StateInterface>(new StatePhoto);
  d_states[StateId::Video] = std::unique_ptr<StateInterface>(new StateVideo);
  d_state = d_states[StateId::Disconnected].get();

  d_timer_recreate_gopro.setCallback([=](){ // by value capture (this and p)
      d_gp.reset(GoProFactory::createGoPro(this,
					   p.model,
					   p.name));
      connectReq();
    });
}

//----------------------------------------------------------------------//
void FastController::connectReq()
{
  d_reqs.emplace_back(Req::Connect);
  processCurrentState();
}

//----------------------------------------------------------------------//
void FastController::takePhotoReq()
{
  d_reqs.emplace_back(Req::Photo);
  processCurrentState();
}

//----------------------------------------------------------------------//
void FastController::toggleRecordingReq()
{
  d_reqs.emplace_back(Req::ToggleRecording);
  processCurrentState();
}

//----------------------------------------------------------------------//
void FastController::setState(StateId id)
{	
  d_state_id = id;
  d_state = d_states[id].get();
  assert(d_state != nullptr);
}

//----------------------------------------------------------------------//
FastController::Req FastController::getRequest()
{
  return d_reqs.front();
}

//----------------------------------------------------------------------//
void FastController::processCurrentState()
{
  assert(d_state != nullptr);
  d_state->process(*this);
}

//----------------------------------------------------------------------//
void FastController::handleCommandSuccessful(GoPro*, Cmd cmd)
{
  std::cout << "D_GP::FastController::handleCommandSuccessful " << std::endl;
  if (!d_gp->hasBufferedReqs())
    {
      assert(!d_reqs.empty());
      d_reqs.pop_front(); // completed last request
    }
  
  switch (cmd)
    {
    case Cmd::SetModePhoto:
      setState(StateId::Photo);
      return;

    case Cmd::SetModeVideo:
      setState(StateId::Video);
      return;

    case Cmd::Connect:
      setState(StateId::Connected);
      return;
			
    case Cmd::SetShutterTrigger:
      if (d_state_id == StateId::Video)
	{
	  d_is_recording = true;
	}
      return;
      
    case Cmd::SetShutterStop:
      if (d_state_id == StateId::Video)
	{
	  d_is_recording = false;
	}
      return;
      
    case Cmd::LiveStream:
      return;

    case Cmd::Unknown:
      assert(false);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
void FastController::handleCommandFailed(GoPro*, Cmd cmd, GPError err)
{
  std::cout << "D_GP::FastController::handleCommandFailed " << std::endl;
  // stop further consequetive failed messages that have been buffered/queued
  d_gp->cancelBufferedCmds(); 

  Req req = d_reqs.front();
  switch (err)
    {
    case GPError::Response:
      // LOG
      d_reqs.pop_front(); // last request failed
      // otherwise, assume the same state
      break;

    case GPError::ResponseData:
      // LOG
      // DO NOT WANT THIS TO HAPPEN
      d_reqs.pop_front(); // last request failed
      // otherwise, assume the same state
      break;
      
    case GPError::Internal:
      // LOG
      d_timer_recreate_gopro.restartMs(5000); // can't reset gopro while in its callback.
      d_reqs.clear();
      d_is_recording = false;
      setState(StateId::Disconnected);
      break;
      
    case GPError::Timeout:
      // LOG
      d_reqs.clear();
      d_is_recording = false;
      setState(StateId::Disconnected);
      connectReq(); // try to reconnect
      break;
    }
  d_owner->handleFailedRequest(this, req);
}

//----------------------------------------------------------------------//
void FastController::StateDisconnected::process(FastController& ctrl)
{
  std::cout << "FastController::StateDisconnected::process" << std::endl;
  assert(ctrl.d_gp != nullptr);

  GoPro& gp = *ctrl.d_gp;
  Req req = ctrl.getRequest();
  switch (req)
    {
    case Req::Unknown:
      assert(false);
      return;
			
    case Req::Connect:
      gp.connect();
      return;
			
    case Req::Photo:
      gp.connect();
      // TO DO: Need to check if currently recording before taking a photo. 
      // If currently recording, the recording would be stopped before changing to photo mode, otherwise it will not work (camera responds with an error).
      gp.setMode(Mode::Photo);
      // If a trigger is called here (to take a picture), the camera does not change mode in time, and so a trigger is set for the current mode
      return;
			
    case Req::ToggleRecording:
      // just goes into recording mode here
      gp.connect();
      // TO DO: Need to check if currently recording before toggling recording. 
      // If currently recording, the recording would be stopped.
      gp.setMode(Mode::Video);
      return;
    };
  assert(false);
}

//----------------------------------------------------------------------//
void FastController::StateConnected::process(FastController& ctrl)
{
  std::cout << "FastController::StateConnected::process" << std::endl;
  assert(ctrl.d_gp != nullptr);

  GoPro& gp = *ctrl.d_gp;
  Req req = ctrl.getRequest();
  switch (req)
    {
    case Req::Unknown:
      assert(false);
      return;
			
    case Req::Connect:
      // already connected
      assert(false);
      return;
			
    case Req::Photo:
      gp.setMode(Mode::Photo);
      // If a trigger is called here (to take a picture), the camera does not change mode in time, and so a trigger is set for the current mode
      return;
			
    case Req::ToggleRecording:
      gp.setMode(Mode::Video);
      gp.setShutter(!ctrl.isRecording());
      ctrl.toggleRecState();
      return;
    };
  assert(false);
}

//----------------------------------------------------------------------//
void FastController::StatePhoto::process(FastController& ctrl)
{
  std::cout << "FastController::StatePhoto::process" << std::endl;
  assert(ctrl.d_gp != nullptr);

  GoPro& gp = *ctrl.d_gp;
  Req req = ctrl.getRequest();
  switch (req)
    {
    case Req::Unknown:
      assert(false);
      return;
			
    case Req::Connect:
      // already connected
      assert(false);
      return;
			
    case Req::Photo:
      gp.setShutter(true); // take pic
      return;
			
    case Req::ToggleRecording:
      gp.setMode(Mode::Video);
      gp.setShutter(!ctrl.isRecording());
      ctrl.toggleRecState();
      return;
    };
  assert(false);
}

//----------------------------------------------------------------------//
void FastController::StateVideo::process(FastController& ctrl)
{
  std::cout << "FastController::StateVideo::process" << std::endl;
  assert(ctrl.d_gp != nullptr);

  GoPro& gp = *ctrl.d_gp;
  Req req = ctrl.getRequest();
  switch (req)
    {
    case Req::Unknown:
      assert(false);
      return;
			
    case Req::Connect:
      // already connected
      assert(false);
      return;
			
    case Req::Photo:
      gp.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
      return;
			
    case Req::ToggleRecording:
      gp.setShutter(!ctrl.isRecording());
      ctrl.toggleRecState();
      return;
    };
  assert(false);
}
