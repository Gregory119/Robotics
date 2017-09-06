#include "dgp_controller.h"

#include "dgp_factory.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

//----------------------------------------------------------------------//
GoProController::GoProController(GoProControllerOwner* o,
				 const GPCtrlParams& p)
  : d_owner(o),
    d_gp(GoProFactory::createGoPro(this, p.model, p.name)),
    d_state(new StateDisconnected)
{
  assert(o!=nullptr);
  
  // add list of possible states
  d_states[GPStateId::Disconnected] = std::unique_ptr<GPState>(new StateDisconnected);
  d_states[GPStateId::Connected] = std::unique_ptr<GPState>(new StateConnected);
  d_states[GPStateId::Photo] = std::unique_ptr<GPState>(new StatePhoto);
  d_states[GPStateId::Video] = std::unique_ptr<GPState>(new StateVideo);
  d_state = d_states[GPStateId::Connected].get();

  d_timer_recreate_gopro.setCallback([this](){
      d_gp.reset(GoProFactory::createGoPro(this,
					   p.model,
					   p.name));
      connectReq();
    });
}

//----------------------------------------------------------------------//
GoProController::~GoProController() = default;

//----------------------------------------------------------------------//
void GoProController::connectReq()
{
  d_reqs.emplace_back(GoProControllerReq::Connect);
  processCurrentState();
}

//----------------------------------------------------------------------//
void GoProController::takePhotoReq()
{
  d_reqs.emplace_back(GoProControllerReq::Photo);
  processCurrentState();
}

//----------------------------------------------------------------------//
void GoProController::toggleRecordingReq()
{
  d_reqs.emplace_back(GoProControllerReq::ToggleRecording);
  processCurrentState();
}

//----------------------------------------------------------------------//
void GoProController::setState(GPStateId id)
{	
  d_state_id = id;
  d_state = d_states[id].get();
  assert(d_state != nullptr);
}

//----------------------------------------------------------------------//
GoProControllerReq GoProController::getRequest()
{
  return d_reqs.front();
}

//----------------------------------------------------------------------//
void StateDisconnected::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);

  GoPro& gp = *ctrl.d_gp;
  GoProControllerReq &req = ctrl.getRequest();
  switch (req)
    {
    case GoProControllerReq::Unknown:
      assert(false);
      return;
			
    case GoProControllerReq::Connect:
      gp.connect();
      gp.setShutter(false); // stop possible recording.
      return;
			
    case GoProControllerReq::Photo:
      gp.connect();
      gp.setShutter(false); // stop possible recording.
      gp.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
      return;
			
    case GoProControllerReq::ToggleRecording:
      gp.connect();
      gp.setShutter(false); // stop possible recording.
      gp.setMode(Mode::Video);
      return;
    };
  assert(false);
}

//----------------------------------------------------------------------//
void StateConnected::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);

  GoPro& gp = *ctrl.d_gp;
  GoProControllerReq &req = ctrl.getRequest();
  switch (req)
    {
    case GoProControllerReq::Unknown:
      assert(false);
      return;
			
    case GoProControllerReq::Connect:
      // already connected
      assert(false);
      return;
			
    case GoProControllerReq::Photo:
      gp.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
      return;
			
    case GoProControllerReq::ToggleRecording:
      gp.setMode(Mode::Video);
      gp.setShutter(!isRecording());
      gp.toggleRecState();
      return;
    };
  assert(false);
}

//----------------------------------------------------------------------//
void StatePhoto::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);

  GoPro& gp = *ctrl.d_gp;
  GoProControllerReq &req = ctrl.getRequest();
  switch (req)
    {
    case GoProControllerReq::Unknown:
      assert(false);
      return;
			
    case GoProControllerReq::Connect:
      // already connected
      assert(false);
      return;
			
    case GoProControllerReq::Photo:
      gp.setShutter(true); // take pic
      return;
			
    case GoProControllerReq::ToggleRecording:
      gp.setMode(Mode::Video);
      gp.setShutter(!isRecording());
      gp.toggleRecState();
      return;
    };
  assert(false);
}

//----------------------------------------------------------------------//
void StateVideo::process(GoProController& ctrl)
{
  assert(ctrl.d_gp != nullptr);

  GoPro& gp = *ctrl.d_gp;
  GoProControllerReq &req = ctrl.getRequest();
  switch (req)
    {
    case GoProControllerReq::Unknown:
      assert(false);
      return;
			
    case GoProControllerReq::Connect:
      // already connected
      assert(false);
      return;
			
    case GoProControllerReq::Photo:
      gp.setMode(Mode::Photo);
      gp.setShutter(true); // take pic
      return;
			
    case GoProControllerReq::ToggleRecording:
      gp.setShutter(!isRecording());
      gp.toggleRecState();
      return;
    };
  assert(false);
}

//----------------------------------------------------------------------//
void GoProController::processCurrentState()
{
  assert(d_state != nullptr);
  d_state->process(*this);
}

//----------------------------------------------------------------------//
void GoProController::handleCommandSuccessful(GoPro*, Cmd cmd)
{
  if (!d_gp->hasBufferedReqs())
    {
      assert(!d_reqs.empty());
      d_reqs.pop_front(); // completed last request
    }
  
  switch (cmd)
    {
    case Cmd::SetModePhoto:
      setState(GPStateId::Photo);
      return;

    case Cmd::SetModeVideo:
      setState(GPStateId::Video);
      return;

    case Cmd::Connect:
      setState(GPStateId::Connected);
      return;
			
    case Cmd::SetShutterTrigger:
      if (d_state_id == GPStateId::Video)
	{
	  d_is_recording = true;
	}
      return;
      
    case Cmd::SetShutterStop:
      if (d_state_id == GPStateId::Video)
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
void GoProController::handleCommandFailed(GoPro*, Cmd cmd, GPError err)
{
  std::cout << "GoProController::handleCommandFailed " << std::endl;
  // stop further consequetive failed messages that have been buffered/queued
  d_gp->cancelBufferedCmds(); 

  GoProControllerReq req = d_reqs.front();
  switch (err)
    {
    case GPError::Response:
      // LOG
      d_reqs.pop_front(); // last request failed
      // otherwise, assume the same state
      break;
      
    case GPError::Internal:
      // LOG
      d_timer_recreate_gopro.restartMs(5000); // can't reset gopro while in its callback.
      d_reqs.clear();
      d_is_recording = false;
      setState(GPStateId::Disconnected);
      break;
      
    case GPError::Timeout:
      // LOG
      d_reqs.clear();
      d_is_recording = false;
      setState(GPStateId::Disconnected);
      connectReq(); // try to reconnect
      break;
    }
  d_owner->handleFailedRequest(this, req);
}
