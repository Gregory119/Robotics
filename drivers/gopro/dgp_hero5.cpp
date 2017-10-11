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
  d_http->get(CmdToUrlConverter::cmdToUrl(Cmd::Connect,
					  CamModel::Hero5,
					  params));
  d_cmd_reqs.push_back(Cmd::Connect);
}

//----------------------------------------------------------------------//
void GoProHero5::status()
{
  d_http->get(CmdToUrlConverter::cmdToUrl(Cmd::Status,
					  CamModel::Hero5));
  d_cmd_reqs.push_back(Cmd::Status);
}

//----------------------------------------------------------------------//
void GoProHero5::setMode(Mode mode)
{
  // LOG
  std::cout << "GoProHero5::setMode()" << std::endl;
  switch (mode)
    {
    case Mode::Photo:
      d_http->get(CmdToUrlConverter::cmdToUrl(Cmd::SetModePhoto,
					      CamModel::Hero5));
      d_cmd_reqs.push_back(Cmd::SetModePhoto);
      return;
      
    case Mode::Video:
      d_http->get(CmdToUrlConverter::cmdToUrl(Cmd::SetModeVideo,
					      CamModel::Hero5));
      d_cmd_reqs.push_back(Cmd::SetModeVideo);
      return;

    case Mode::Video:
      d_http->get(CmdToUrlConverter::cmdToUrl(Cmd::SetModeMultiShot,
					      CamModel::Hero5));
      d_cmd_reqs.push_back(Cmd::SetModeMultiShot);
      return;

    case Mode::Unknown:
      assert(false);
      return;
    }
  assert(false);
  d_owner->handleCommandFailed(this, Cmd::Unknown, GPError::Internal);
}

//----------------------------------------------------------------------//
void GoProHero5::setShutter(bool state)
{
  // LOG
  std::cout << "GoProHero5::setShutter()" << std::endl;
  if (state)
    {
      d_http->get(CmdToUrlConverter::cmdToUrl(Cmd::SetShutterTrigger,
					      CamModel::Hero5));
      d_cmd_reqs.push_back(Cmd::SetShutterTrigger);
    }
  else
    {
      d_http->get(CmdToUrlConverter::cmdToUrl(Cmd::SetShutterStop,
					      CamModel::Hero5));
      d_cmd_reqs.push_back(Cmd::SetShutterStop);
    }
}

//----------------------------------------------------------------------//
void GoProHero5::handleFailed(C_HTTP::HttpOperations* http,
			      C_HTTP::HttpOpError error)
{
  // sent command was unsuccessful
  Cmd cmd = CmdToUrlConverter::urlToCmd(http->getUrl(), CamModel::Hero5);

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
  Cmd cmd = d_cmd_reqs.front();
  d_cmd_reqs.pop_front();
  
  if (code >= static_cast<C_HTTP::HttpResponseCode>(C_HTTP::ResponseCode::BadRequest))
    {
      // not successful
      d_owner->handleCommandFailed(this, cmd, GPError::Response);
      return;
    }

  switch (cmd)
    {
    case Cmd::Connect:
    case Cmd::SetModePhoto:
    case Cmd::SetModeVideo:
    case Cmd::SetShutterTrigger:
    case Cmd::SetShutterStop:
      d_owner->handleCommandSuccessful(this,
				       cmd);
      return;

    case Cmd::Status:
      {
	std::string body = std::string(body.begin(),body.end());
	if (parseStatus(body))
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
      
    case Cmd::LiveStream:
      // nothing (internal command)
      return;

    case Cmd::Unknown:
      assert(false);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
bool GoProHero5::parseStatus(const std::string& body)
{
  if (body.empty())
    {
      return false;
    }

  // all appear first in the 'status' JSON node
  size_t pos_mode = body.find("\"43\":"); 
  size_t pos_rec = body.find("\"8\":");
  size_t pos_stream = body.find("\"32\":"); 

  if (pos_mode == std::string::npos ||
      pos_rec == std::string::npos ||
      pos_stream == std::string::npos)
    {
      return false;
    }

  try
    {
      d_status.mode = std::stoi(body.substr(pos_mode+5,1));
      
      int rec = std::stoi(body.substr(pos_rec+5,1));
      if (rec != 1 && rec != 0)
	{
	  return false;
	}
      d_status.is_recording = (rec != 0);

      int stream = std::stoi(body.substr(pos_stream+5,1));
      if (stream != 1 && stream != 0)
	{
	  return false;
	}
      d_status.is_streaming = (stream != 0);
    }
  catch (...)
    {
      d_status.clear();
      return false;
    }
  
  return true;
}

//----------------------------------------------------------------------//
bool GoProHero5::handleTimeOut(const KERN::KernelTimer& timer)
{
  if (timer.is(d_timer_stream_check))
    {
      d_http->get(CmdToUrlConverter::cmdToUrl(Cmd::LiveStream,
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
  d_http->get(CmdToUrlConverter::cmdToUrl(Cmd::LiveStream,
					  CamModel::Hero5));
  d_cmd_reqs.push_back(Cmd::StartLiveStream);
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
				   Cmd::StopLiveStream);
    
  // STILL TO DO
  // stop waiting on state of pi analogue connection
  // stop requesting stream on a timer
  // stop the terminal process of omxplayer
}
