#include "dgp_hero5.h"

#include "dgp_utils.h"

#include <cassert>

using namespace D_GP;

static const int s_http_timeout_ms = 3000;
static const int s_connection_check_ms = 5000;

//----------------------------------------------------------------------//
GoProHero5::GoProHero5(GoProOwner* o)
  : GoPro(o)
{
  assert(o != nullptr);
  d_http.reset(new C_HTTP::HttpOperations(this));
}

//----------------------------------------------------------------------//
void GoProHero5::connectWithName(const std::string& name)
{
  if (name.empty())
    {
      // log
      assert(false);
      return;
    }
  d_connect_name = name;
  connect();
}

//----------------------------------------------------------------------//
void GoProHero5::connect()
{
  if (!d_connected)
    {
      d_timer_connect_check.disable();
      d_http->cancelBufferedReqs(); // buffered reqs won't go through if disconnected
    }

  if (!d_http->init(s_http_timeout_ms))
    {
      d_owner->handleFailedCommand(this,Cmd::Connect);
      return;
    }

  std::vector<std::string> params = {d_connect_name};
  if (!d_http->get(HttpCmdConverter::cmdToUrl(Cmd::Connect,
					      CamModel::Hero5,
					      params)))
    {
      d_owner->handleFailedCommand(this, Cmd::Connect);
      return;
    }
}

//----------------------------------------------------------------------//
void GoProHero5::setMode(Mode mode)
{
  bool ret = false;
  switch (mode)
    {
    case Mode::Photo:
      ret = d_http->get(HttpCmdConverter::cmdToUrl(Cmd::SetModePhoto,
						   CamModel::Hero5));
      break;
      
    case Mode::Video:
      ret = d_http->get(HttpCmdConverter::cmdToUrl(Cmd::SetModeVideo,
						   CamModel::Hero5));
      break;
    }
  
  if (!ret)
    {
      d_owner->handleFailedCommand(this,Cmd::SetMode);
      return;
    }
}

//----------------------------------------------------------------------//
void GoProHero5::setShutter(bool state)
{
  bool ret = false;

  if (state)
    {
      ret = d_http->get(HttpCmdConverter::cmdToUrl(Cmd::SetShutterTrigger,
						   CamModel::Hero5));
    }
  else
    {
      ret = d_http->get(HttpCmdConverter::cmdToUrl(Cmd::SetShutterStop,
						   CamModel::Hero5));
    }

  if (!ret)
    {
      d_owner->handleFailedCommand(this,Cmd::SetShutter);
      return;
    }
}

//----------------------------------------------------------------------//
void GoProHero5::handleFailed(HttpOperations* http,
			      C_HTTP::HttpOpError error)
{
  // sent command was unsuccessful
  Cmd cmd = HttpCmdConverter::urlToType(http->getUrl());

  switch (error)
    {
    case C_HTTP::HttpOpError::Internal:
      d_owner->handleFailedCommand(this,cmd);
      return;

    case C_HTTP::HttpOpError::Timeout:
      {
	bool connection_down = d_connected;
	d_connected = false;
	connect(); // Attempt to reconnect (this will occur every timeout until connected).
	// Internally stops connection check timer.
	if (connection_down)
	  {
	    d_owner->handleDisconnected(this,cmd);
	  }
      }
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
void GoProHero5::handleResponse(HttpOperations* http,
				C_HTTP::HttpResponseCode code,
				const std::vector<std::string>& headers,
				const std::vector<char>& body)
{
  Cmd cmd = HttpCmdConverter::urlToType(http->getUrl());
  
  if (code >= static_cast<C_HTTP::HttpResponseCode>(C_HTTP::ResponseCode::BadRequest))
    {
      // not successful
      d_owner->handleCommandFailed(this, cmd);
      return;
    }

  // successful response => connected
  bool connection_up = !d_connected;
  d_connected = true;
  switch (cmd)
    {
    case Cmd::Connect:
      if (connection_up)
	{
	  d_timer_connect_check.restartTimerMs(s_connection_check_ms);
	  d_owner->handleCommandSuccessful(this,
				       cmd);
	}
      return;
      
    case Cmd::SetModePhoto:
    case Cmd::SetModeVideo:
    case Cmd::SetShutterTrigger:
    case Cmd::SetShutterStop:
      d_owner->handleCommandSuccessful(this,
				       cmd);
      return;
      
    case Cmd::LiveStream:
      // STILL TO DO
      assert(false);
      return;

      // case Cmd::Status => save to structure which can get read with a get function
      
    case Cmd::Unknown:
      assert(false);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
bool GoProHero5::handleTimeOut(const KernelTimer& timer)
{
  if (timer.is(d_timer_connect_check))
    {
      connect();
      return true;
    }
  
  return false;
}

//----------------------------------------------------------------------//
void GoProHero5::startLiveStream()
{
  assert(false);
  // STILL TO DO
  // possibly wait on state of pi analogue connection
  // send stream request command on a timer
  // start a terminal process of omxplayer to send udp stream to the pi analogue output
}

//----------------------------------------------------------------------//
void GoProHero5::stopLiveStream()
{
  assert(false);
  // STILL TO DO
  // stop waiting on state of pi analogue connection
  // stop requesting stream on a timer
  // stop the terminal process of omxplayer
}

//----------------------------------------------------------------------//
//GoProHero5::
