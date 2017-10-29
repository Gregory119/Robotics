#include "dgp_modecontroller.h"

#include "dgp_factory.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

//----------------------------------------------------------------------//
ModeController::ModeController(Owner* o,
			       const CtrlParams& p)
  : d_owner(o),
    d_gp(GoProFactory::createGoPro(this, p.model, p.name))
{
  assert(o!=nullptr);
  assert(d_gp!=nullptr);
}

//----------------------------------------------------------------------//
void ModeController::connect()
{
  // does not need to check status
  d_gp->connect();
  d_reqs.push_back(Req::Connect);
}

//----------------------------------------------------------------------//
void ModeController::nextMode()
{
  d_gp->status();
  d_reqs.push_back(Req::NextMode);
}
  
//----------------------------------------------------------------------//
void ModeController::trigger()
{
  d_gp->status();
  d_reqs.push_back(Req::Trigger);
}

//----------------------------------------------------------------------//
void ModeController::startStream()
{
  d_gp->status();
  d_reqs.push_back(Req::StartStream);
}

//----------------------------------------------------------------------//
void ModeController::stopStream()
{
  d_gp->status();
  d_reqs.push_back(Req::StopStream);
}

//----------------------------------------------------------------------//
void ModeController::handleCommandSuccessful(GoPro*, GoPro::Cmd cmd)
{
  std::cout << "D_GP::ModeController::handleCommandSuccessful " << std::endl;
  assert(!d_reqs.empty());
  
  switch (cmd)
    {
    case GoPro::Cmd::Status:
      processStatus(); // request still in process
      return;

    case GoPro::Cmd::Connect:
    case GoPro::Cmd::SetModePhotoSingle:
    case GoPro::Cmd::SetModePhotoContinuous:
    case GoPro::Cmd::SetModePhotoNight:
    case GoPro::Cmd::SetModeVideoNormal:
    case GoPro::Cmd::SetModeVideoTimeLapse:
    case GoPro::Cmd::SetModeVideoPlusPhoto:
    case GoPro::Cmd::SetModeVideoLooping:
    case GoPro::Cmd::SetModeMultiShotBurst:
    case GoPro::Cmd::SetModeMultiShotTimeLapse:
    case GoPro::Cmd::SetModeMultiShotNightLapse:
    case GoPro::Cmd::SetShutterTrigger:
    case GoPro::Cmd::SetShutterStop:
    case GoPro::Cmd::StartLiveStream:
    case GoPro::Cmd::StopLiveStream:
      {
	Req req = d_reqs.front();
	d_reqs.pop_front(); // completed last request
	if (d_owner != nullptr)
	  {
	    d_owner->handleSuccessfulRequest(this, req);
	  }
      }
      return;
      
    case GoPro::Cmd::Unknown:
      assert(false);
      sendFailedRequest();
      return;
    }
  assert(false);
  sendFailedRequest();
}

//----------------------------------------------------------------------//
void ModeController::sendFailedRequest()
{
  d_gp->cancelBufferedCmds();
  Req req = d_reqs.front();
  d_reqs.clear();
  if (d_owner != nullptr)
    {
      d_owner->handleFailedRequest(this, req);
    }
}

//----------------------------------------------------------------------//
void ModeController::handleCommandFailed(GoPro*,
					 GoPro::Cmd cmd,
					 GoPro::Error err)
{
  std::cout << "D_GP::ModeController::handleCommandFailed " << std::endl;
  // stop further consequetive failed messages that have been buffered/queued
  sendFailedRequest();
}

//----------------------------------------------------------------------//
void ModeController::processStatus()
{
  Req req = d_reqs.front();
  switch (req)
    {
    case Req::NextMode:
      if (!processNextMode())
	{
	  sendFailedRequest();
	}
      return;
      
    case Req::Trigger:
      d_gp->setShutter(!d_gp->getStatus().d_is_recording);
      return;
      
    case Req::StartStream:
      if (!d_gp->getStatus().d_is_streaming)
	{
	  d_gp->startLiveStream();
	}
      return;
      
    case Req::StopStream:
      if (d_gp->getStatus().d_is_streaming)
	{
	  d_gp->stopLiveStream();
	}
      return;

    case Req::Connect: // does not need to check status before request
    case Req::Unknown:
      assert(false);
      sendFailedRequest();
      return;
    }
  assert(false);
  sendFailedRequest();
}

//----------------------------------------------------------------------//
bool ModeController::processNextMode()
{
  const Status& status = d_gp->getStatus();
  if (status.d_is_recording)
    {
      d_gp->setShutter(false);
    }
  
  switch (status.d_mode)
    {
    case Mode::VideoNormal:
    case Mode::VideoTimeLapse:
    case Mode::VideoPlusPhoto:
    case Mode::VideoLooping:
      d_gp->setMode(Mode::PhotoContinuous);
      return true;
    
    case Mode::PhotoSingle:
    case Mode::PhotoContinuous:
    case Mode::PhotoNight:
      d_gp->setMode(Mode::MultiShotBurst);
      return true;
      
    case Mode::MultiShotBurst:
      d_gp->setMode(Mode::MultiTimeLapse);
      return true;
      
    case Mode::MultiTimeLapse:
    case Mode::MultiNightLapse:
      d_gp->setMode(Mode::VideoNormal);
      return true;
	
    case Mode::Unknown:
      assert(false);
      return false;
    }
  assert(false);
  return false;
}
