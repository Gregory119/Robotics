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
  startRequest(Req::Connect);
}

//----------------------------------------------------------------------//
void ModeController::nextMode()
{
  startRequest(Req::NextMode);
}
  
//----------------------------------------------------------------------//
void ModeController::trigger()
{
  startRequest(Req::Trigger);
}

//----------------------------------------------------------------------//
void ModeController::startStream()
{
  startRequest(Req::StartStream);
}

//----------------------------------------------------------------------//
void ModeController::stopStream()
{
  startRequest(Req::StopStream);
}

//----------------------------------------------------------------------//
void ModeController::handleCommandSuccessful(GoPro*, GoPro::Cmd cmd)
{
  std::cout << "D_GP::ModeController::handleCommandSuccessful " << std::endl;
  assert(!d_reqs.empty());
  processReqWithLastCmd(cmd);
}

//----------------------------------------------------------------------//
void ModeController::handleCommandFailed(GoPro*,
					 GoPro::Cmd cmd,
					 GoPro::Error err)
{
  std::cout << "D_GP::ModeController::handleCommandFailed " << std::endl;
  // stop further consequetive failed messages that have been buffered/queued
  ownerFailedRequest();
}

//----------------------------------------------------------------------//
void ModeController::handleStreamDown(GoPro*)
{
  if (d_owner != nullptr)
    {
      d_owner->handleFailedRequest(this, Req::StartStream);
    }
}

//----------------------------------------------------------------------//
void ModeController::startRequest(Req req)
{
  bool is_processing_reqs = !d_reqs.empty();
  d_reqs.push_back(req);
  if (!is_processing_reqs)
    {
      processNextReq();
    }
}

//----------------------------------------------------------------------//
void ModeController::processNextReq()
{
  if (d_reqs.empty())
    {
      return;
    }
    
  processReqWithLastCmd(GoPro::Cmd::Unknown);
}

//----------------------------------------------------------------------//
void ModeController::processReqWithLastCmd(GoPro::Cmd last_cmd)
{
  assert(!d_reqs.empty());
  switch (d_reqs.front())
    {
    case Req::NextMode:
      internalNextMode(last_cmd);
      return;
      
    case Req::Trigger:
      internalTrigger(last_cmd);
      return;
      
    case Req::StartStream:
      internalStartStream(last_cmd);
      return;
      
    case Req::StopStream:
      internalStopStream(last_cmd);
      return;

    case Req::Connect:
      internalConnect(last_cmd);
      return;
      
    case Req::Unknown:
      assert(false);
      ownerInternalFailure();
      return;
    }
  assert(false);
  ownerInternalFailure();
}

//----------------------------------------------------------------------//
void ModeController::nextModeCmdsAfterStatus()
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
      return;
    
    case Mode::PhotoSingle:
    case Mode::PhotoContinuous:
    case Mode::PhotoNight:
      d_gp->setMode(Mode::MultiShotBurst);
      return;
      
    case Mode::MultiShotBurst:
      d_gp->setMode(Mode::MultiTimeLapse);
      return;
      
    case Mode::MultiTimeLapse:
    case Mode::MultiNightLapse:
      d_gp->setMode(Mode::VideoNormal);
      return;
	
    case Mode::Unknown:
      assert(false);
      ownerInternalFailure();
      return;
    }
  assert(false);
  ownerInternalFailure();
}

//----------------------------------------------------------------------//
void ModeController::internalConnect(GoPro::Cmd last_cmd)
{
  assert(d_reqs.front() == Req::Connect);

  switch (last_cmd)
    {
    case GoPro::Cmd::Unknown:
      d_gp->connect();
      return;

    case GoPro::Cmd::Connect:
      ownerSuccessfulRequest();
      processNextReq();
      return;
      
    case GoPro::Cmd::Status:
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
    case GoPro::Cmd::SetBitRate:
      assert(false);
      ownerInternalFailure();
      return;
    }
  assert(false);
  ownerInternalFailure();
}

//----------------------------------------------------------------------//
void ModeController::internalNextMode(GoPro::Cmd last_cmd)
{
  assert(d_reqs.front() == Req::NextMode);

  switch (last_cmd)
    {
    case GoPro::Cmd::Unknown:
      d_gp->status();
      return;

    case GoPro::Cmd::Status:
      nextModeCmdsAfterStatus();
      return;

    case GoPro::Cmd::SetShutterStop:
      // possibly used in the list of commands after the status command was successful
      return;
      
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
      ownerSuccessfulRequest();
      processNextReq();
      return;

    case GoPro::Cmd::Connect:
    case GoPro::Cmd::SetShutterTrigger:
    case GoPro::Cmd::StartLiveStream:
    case GoPro::Cmd::StopLiveStream:
    case GoPro::Cmd::SetBitRate:
      assert(false);
      ownerInternalFailure();
      return;
    }
  assert(false);
  ownerInternalFailure();
}

//----------------------------------------------------------------------//
void ModeController::internalTrigger(GoPro::Cmd last_cmd)
{
  assert(d_reqs.front() == Req::Trigger);

  switch (last_cmd)
    {
    case GoPro::Cmd::Unknown:
      d_gp->status();
      return;

    case GoPro::Cmd::Status:
      d_gp->setShutter(!d_gp->getStatus().d_is_recording);
      return;

    case GoPro::Cmd::SetShutterTrigger:
    case GoPro::Cmd::SetShutterStop:
      ownerSuccessfulRequest();
      processNextReq();
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
    case GoPro::Cmd::StartLiveStream:
    case GoPro::Cmd::StopLiveStream:
    case GoPro::Cmd::SetBitRate:
      assert(false);
      ownerInternalFailure();
      return;
    }
  assert(false);
  ownerInternalFailure();
}

//----------------------------------------------------------------------//
void ModeController::internalStartStream(GoPro::Cmd last_cmd)
{
  assert(d_reqs.front() == Req::StartStream);

  switch (last_cmd)
    {
    case GoPro::Cmd::Unknown:
      d_gp->status();
      return;

    case GoPro::Cmd::Status:
      // after status command is successful:
      //if (!d_gp->getStatus().d_is_streaming)
      //{
      // make sure the stream is started on every request
      d_gp->setBitRatePerSecond(2400000); // 2.4 Mbps
      d_gp->startLiveStream();
      //}
      return;

    case GoPro::Cmd::SetBitRate:
      // use in list of commands after a successful status command
      return;
      
    case GoPro::Cmd::StartLiveStream:
      ownerSuccessfulRequest();
      processNextReq();
      return;
	    
    case GoPro::Cmd::SetShutterTrigger:
    case GoPro::Cmd::SetShutterStop:
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
    case GoPro::Cmd::StopLiveStream:
      assert(false);
      ownerInternalFailure();
      return;
    }
  assert(false);
  ownerInternalFailure();
}

//----------------------------------------------------------------------//
void ModeController::internalStopStream(GoPro::Cmd last_cmd)
{
  assert(d_reqs.front() == Req::StopStream);

  switch (last_cmd)
    {
    case GoPro::Cmd::Unknown:
      d_gp->status();
      return;

    case GoPro::Cmd::Status:
      if (d_gp->getStatus().d_is_streaming)
	{
	  d_gp->stopLiveStream();
	}
      return;

    case GoPro::Cmd::StopLiveStream:
      ownerSuccessfulRequest();
      return;
      
    case GoPro::Cmd::SetBitRate:
    case GoPro::Cmd::SetShutterTrigger:
    case GoPro::Cmd::SetShutterStop:
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
    case GoPro::Cmd::StartLiveStream:
      assert(false);
      ownerInternalFailure();
      return;
    }
  assert(false);
  ownerInternalFailure();
}

//----------------------------------------------------------------------//
void ModeController::ownerFailedRequest()
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
void ModeController::ownerInternalFailure()
{
  d_gp->cancelBufferedCmds();
  if (d_owner != nullptr)
    {
      d_owner->handleInternalFailure(this);
    }
}

//----------------------------------------------------------------------//
void ModeController::ownerSuccessfulRequest()
{
  Req req = d_reqs.front();
  d_reqs.pop_front(); // completed last request
  if (d_owner != nullptr)
    {
      d_owner->handleSuccessfulRequest(this, req);
    }
}
