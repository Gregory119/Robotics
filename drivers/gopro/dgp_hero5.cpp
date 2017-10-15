#include "dgp_hero5.h"

#include "dgp_utils.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

static const int s_http_timeout_ms = 3000;
static const int s_connection_check_ms = 5000;

//----------------------------------------------------------------------//
GoProHero5::GoProHero5(GoProOwner* o, const std::string& name)
  : GoPro(o),
    d_timer_stream_check(KERN::KernelTimer(this))
{
  assert(o != nullptr);
  d_http.reset(new C_HTTP::HttpOperations(this));

  assert(!name.empty());
  d_connect_name = name;
}

//----------------------------------------------------------------------//
void GoProHero5::connect()
{
  // LOG
  std::cout << "GoProHero5::connect()" << std::endl;

  // will return true if already initialized
  bool success = d_http->init(s_http_timeout_ms);
  if (!success)
    {
      // LOG
      assert(false); // this should not happen
      return;
    }
  
  std::vector<std::string> params = {d_connect_name};
  d_http->get(Utils::cmdToUrl(GoPro::Cmd::Connect,
			      CamModel::Hero5,
			      params));
  d_cmd_reqs.push_back(GoPro::Cmd::Connect);
}

//----------------------------------------------------------------------//
void GoProHero5::status()
{
  requestCmd(GoPro::Cmd::Status);
}

//----------------------------------------------------------------------//
void GoProHero5::setMode(Mode mode, int sub_mode)
{
  // LOG
  std::cout << "GoProHero5::setMode()" << std::endl;
  switch (mode)
    {
    case Mode::VideoNormal:
      requestCmd(GoPro::Cmd::SetModeVideoNormal);
      return;
      
    case Mode::VideoTimeLapse:
      requestCmd(GoPro::Cmd::SetModeVideoTimeLapse);
      return;
      
    case Mode::VideoPlusPhoto:
      requestCmd(GoPro::Cmd::SetModeVideoPlusPhoto);
      return;
      
    case Mode::VideoLooping:
      requestCmd(GoPro::Cmd::SetModeVideoLooping);
      return;
      
    case Mode::PhotoSingle:
      requestCmd(GoPro::Cmd::SetModePhotoSingle);
      return;
      
    case Mode::PhotoContinuous:
      requestCmd(GoPro::Cmd::SetModePhotoContinuous);
      return;
      
    case Mode::PhotoNight:
      requestCmd(GoPro::Cmd::SetModePhotoNight);
      return;
      
    case Mode::MultiShotBurst:
      requestCmd(GoPro::Cmd::SetModeMultiShotBurst);
      return;
      
    case Mode::MultiTimeLapse:
      requestCmd(GoPro::Cmd::SetModeMultiShotTimeLapse);
      return;
      
    case Mode::MultiNightLapse:
      requestCmd(GoPro::Cmd::SetModeMultiShotNightLapse);
      return;
	
    case Mode::Unknown:
      assert(false);
      d_owner->handleCommandFailed(this, GoPro::Cmd::Unknown, GPError::Internal);
      return;
    }
  assert(false);
  d_owner->handleCommandFailed(this, GoPro::Cmd::Unknown, GPError::Internal);
}

//----------------------------------------------------------------------//
void GoProHero5::setShutter(bool state)
{
  // LOG
  std::cout << "GoProHero5::setShutter()" << std::endl;
  if (state)
    {
      d_http->get(Utils::cmdToUrl(GoPro::Cmd::SetShutterTrigger,
					      CamModel::Hero5));
      d_cmd_reqs.push_back(GoPro::Cmd::SetShutterTrigger);
    }
  else
    {
      d_http->get(Utils::cmdToUrl(GoPro::Cmd::SetShutterStop,
					      CamModel::Hero5));
      d_cmd_reqs.push_back(GoPro::Cmd::SetShutterStop);
    }
}

//----------------------------------------------------------------------//
void GoProHero5::handleFailed(C_HTTP::HttpOperations* http,
			      C_HTTP::HttpOpError error)
{
  // sent command was unsuccessful
  GoPro::Cmd cmd = Utils::urlToCmd(http->getUrl(), CamModel::Hero5);

  switch (error)
    {
    case C_HTTP::HttpOpError::Internal:
      // LOG
      d_owner->handleCommandFailed(this, cmd, GPError::Internal);
      return;

    case C_HTTP::HttpOpError::Timeout:
      // LOG
      d_owner->handleCommandFailed(this, cmd, GPError::Timeout);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
bool GoProHero5::hasBufferedReqs()
{
  return d_http->hasBufferedReqs();
}

//----------------------------------------------------------------------//
void GoProHero5::cancelBufferedCmds()
{
  if (!d_http->hasBufferedReqs())
    {
      return;
    }

  // Why not to clear the reqs list at this point:
  // If a request is being processed here, and there are no buffers,
  // then the request being processed is the front command request.
  // This request must not be cleared because it is needed for the handle call backs.
#ifndef RELEASE
  if (!d_cmd_reqs.empty())
    {
      assert(d_cmd_reqs.size() == 1);
    }
#endif
  d_http->cancelBufferedReqs();
}

//----------------------------------------------------------------------//
void GoProHero5::handleResponse(C_HTTP::HttpOperations* http,
				C_HTTP::HttpResponseCode code,
				const std::vector<std::string>& headers,
				const std::vector<char>& body)
{
  std::cout << "GoProHero5::handleResponse" << std::endl;

  assert(!d_cmd_reqs.empty());
  GoPro::Cmd cmd = d_cmd_reqs.front();
  d_cmd_reqs.pop_front();
  
  if (code >= static_cast<C_HTTP::HttpResponseCode>(C_HTTP::ResponseCode::BadRequest))
    {
      // not successful
      d_owner->handleCommandFailed(this, cmd, GPError::Response);
      return;
    }

  switch (cmd)
    {
    case GoPro::Cmd::Connect:
    case GoPro::Cmd::SetModePhoto:
    case GoPro::Cmd::SetModeVideo:
    case GoPro::Cmd::SetShutterTrigger:
    case GoPro::Cmd::SetShutterStop:
      d_owner->handleCommandSuccessful(this,
				       cmd);
      return;

    case GoPro::Cmd::Status:
      {
	std::string body = std::string(body.begin(),body.end());
	if (d_status.loadStr(body, CamModel::Hero5))
	  {
	    d_owner->handleCommandSuccessful(this,
					     cmd);
	    return;
	  }
	// LOG
	std::cout << "GoProHero5::handleResponse - failed to parse response. Response body was: \n"
		  << body << std::endl;
	d_owner->handleCommandFailed(this, cmd, GPError::ResponseData);
      }
      return;
      
    case GoPro::Cmd::LiveStream:
      // nothing (internal command)
      return;

    case GoPro::Cmd::Unknown:
      assert(false);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
bool GoProHero5::handleTimeOut(const KERN::KernelTimer& timer)
{
  if (timer.is(d_timer_stream_check))
    {
      d_http->get(Utils::cmdToUrl(GoPro::Cmd::LiveStream,
					      CamModel::Hero5));
      return true;
    }
  
  return false;
}

//----------------------------------------------------------------------//
void GoProHero5::startLiveStream()
{
  assert(false);
  // for now just polling stream and running omxplayer from script
  d_http->get(Utils::cmdToUrl(GoPro::Cmd::LiveStream,
					  CamModel::Hero5));
  d_cmd_reqs.push_back(GoPro::Cmd::StartLiveStream);
  d_timer_stream_check.restartMs(5000);
  
  // STILL TO DO
  // possibly wait on state of pi analogue connection
  // send stream request command on a timer
  // start a terminal process of omxplayer to send udp stream to the pi analogue output
}

//----------------------------------------------------------------------//
void GoProHero5::stopLiveStream()
{
  assert(false);
  // for now just stop polling
  d_timer_stream_check.disable();
  d_owner->handleCommandSuccessful(this,
				   GoPro::Cmd::StopLiveStream);
    
  // STILL TO DO
  // stop waiting on state of pi analogue connection
  // stop requesting stream on a timer
  // stop the terminal process of omxplayer
}

//----------------------------------------------------------------------//
void GoProHero5::requestCmd(GoPro::Cmd cmd)
{
  d_http->get(Utils::cmdToUrl(cmd,
			      CamModel::Hero5));
  d_cmd_reqs.push_back(cmd);
}
