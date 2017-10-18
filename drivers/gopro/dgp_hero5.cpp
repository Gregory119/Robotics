#include "dgp_hero5.h"

#include "dgp_utils.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

static const int s_http_timeout_ms = 3000;
static const int s_connection_check_ms = 5000;

//----------------------------------------------------------------------//
GoProHero5::GoProHero5(GoPro::Owner* o, const std::string& name)
  : GoPro(o)
{
  assert(o != nullptr);
  d_http.reset(new C_HTTP::HttpOperations(this));

  assert(!name.empty());
  d_connect_name = name;

  d_timer_stream.setCallback([this](){
      startLiveStream();
      // use omxplayer to output video on analog connection
    });

  // Will call failure callback if failed
  d_http->init(s_http_timeout_ms);
}

//----------------------------------------------------------------------//
void GoProHero5::connect()
{
  // LOG
  std::cout << "GoProHero5::connect()" << std::endl;
  
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
void GoProHero5::setMode(Mode mode)
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
      d_owner->handleCommandFailed(this, GoPro::Cmd::Unknown, GoPro::Error::Internal);
      return;
    }
  assert(false);
  d_owner->handleCommandFailed(this, GoPro::Cmd::Unknown, GoPro::Error::Internal);
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
  GoPro::Cmd cmd = d_cmd_reqs.front();
  d_cmd_reqs.pop_front();
  
  switch (error)
    {
    case C_HTTP::HttpOpError::Internal:
      // LOG
      d_owner->handleCommandFailed(this, cmd, GoPro::Error::Internal);
      return;

    case C_HTTP::HttpOpError::Timeout:
      // LOG
      d_owner->handleCommandFailed(this, cmd, GoPro::Error::Timeout);
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
      // Why not to clear the reqs list at this point:
      // If a request is being processed here, and there are no buffers,
      // then the request being processed is the front command request.
      // This request must not be cleared because it is needed for the handle call backs.
#ifndef RELEASE
      if (d_http->isProcessingReq())
	{
	  assert(d_cmd_reqs.size() == 1);
	}
      else
	{
	  assert(d_cmd_reqs.empty());
	}
#endif
      return;
    }

  if (d_http->isProcessingReq())
    {
      assert(d_cmd_reqs.size() > 1); // one processing and at least one buffered
      d_cmd_reqs.erase(++d_cmd_reqs.begin(),d_cmd_reqs.end());
    }
  
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
      d_owner->handleCommandFailed(this, cmd, GoPro::Error::Response);
      return;
    }

  switch (cmd)
    {
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
      d_owner->handleCommandSuccessful(this,
				       cmd);
      return;

    case GoPro::Cmd::StartLiveStream:
      if (!d_is_streaming)
	{
	  d_is_streaming = true;
	  d_owner->handleCommandSuccessful(this, cmd);
	}
      // else continue streaming with timer requests
      return;
      
    case GoPro::Cmd::Status:
      {
	if (body.size() == 0)
	  {
	    d_owner->handleCommandSuccessful(this,
					     cmd);
	    return;
	  }
	
	std::string body_str = std::string(body.begin(),body.end());
	if (d_status.loadStr(body_str, CamModel::Hero5))
	  {
	    d_status.print();
	    d_owner->handleCommandSuccessful(this,
					     cmd);
	    return;
	  }
	// LOG
	std::cout << "GoProHero5::handleResponse - failed to parse response. Response body was: \n"
		  << body_str << std::endl;
	d_owner->handleCommandFailed(this, cmd, GoPro::Error::ResponseData);
      }
      return;

    case GoPro::Cmd::StopLiveStream: // should not be handled here
    case GoPro::Cmd::Unknown:
      assert(false);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
void GoProHero5::startLiveStream()
{
  requestCmd(GoPro::Cmd::StartLiveStream);
  if (!d_is_streaming)
    {
      d_timer_stream.restartMsIfNotSetElseDisabled(5000);
    }
}

//----------------------------------------------------------------------//
void GoProHero5::stopLiveStream()
{
  d_timer_stream.disable();
  d_is_streaming = false;
  d_owner->handleCommandSuccessful(this, GoPro::Cmd::StopLiveStream);
}

//----------------------------------------------------------------------//
void GoProHero5::requestCmd(GoPro::Cmd cmd)
{
  d_http->get(Utils::cmdToUrl(cmd,
			      CamModel::Hero5));
  d_cmd_reqs.push_back(cmd);
}
