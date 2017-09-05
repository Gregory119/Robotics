#include "dgp_hero5.h"

#include "dgp_utils.h"

#include <cassert>

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
  setName(name);
}

//----------------------------------------------------------------------//
void GoProHero5::setName(const std::string& name)
{
  assert(!name.empty());
  d_name = name;
}

//----------------------------------------------------------------------//
void GoProHero5::connect()
{
  if (!d_connected)
    {
      d_http->cancelBufferedReqs(); // buffered reqs won't go through if disconnected
    }

	// will return true if already initialized
	bool success = d_http->init(s_http_timeout_ms);
	if (!success)
		{
			// LOG
			assert(success); // this should not happen
			return;
		}
	
  std::vector<std::string> params = {d_connect_name};
  d_http->get(HttpCmdConverter::cmdToUrl(Cmd::Connect,
					 CamModel::Hero5,
					 params));
}

//----------------------------------------------------------------------//
void GoProHero5::setMode(Mode mode)
{
  switch (mode)
    {
    case Mode::Photo:
      d_http->get(HttpCmdConverter::cmdToUrl(Cmd::SetModePhoto,
					     CamModel::Hero5));
      return;
      
    case Mode::Video:
      d_http->get(HttpCmdConverter::cmdToUrl(Cmd::SetModeVideo,
					     CamModel::Hero5));
      return;

    case Mode::Unknown:
      assert(false);
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
void GoProHero5::setShutter(bool state)
{
  if (state)
    {
      d_http->get(HttpCmdConverter::cmdToUrl(Cmd::SetShutterTrigger,
					     CamModel::Hero5));
    }
  else
    {
      d_http->get(HttpCmdConverter::cmdToUrl(Cmd::SetShutterStop,
					     CamModel::Hero5));
    }
}

//----------------------------------------------------------------------//
void GoProHero5::handleFailed(C_HTTP::HttpOperations* http,
			      C_HTTP::HttpOpError error)
{
  // sent command was unsuccessful
  Cmd cmd = HttpCmdConverter::urlToCmd(http->getUrl(), CamModel::Hero5);

  switch (error)
    {
    case C_HTTP::HttpOpError::Internal:
      d_owner->handleCommandFailed(this, cmd, GPError::Internal);
      return;

    case C_HTTP::HttpOpError::Timeout:
      {
	d_connected = false;
	d_owner->handleCommandFailed(this, cmd, GPError::Timeout);
      }
      return;
    }
  assert(false);
}

//----------------------------------------------------------------------//
void GoProHero5::handleResponse(C_HTTP::HttpOperations* http,
				C_HTTP::HttpResponseCode code,
				const std::vector<std::string>& headers,
				const std::vector<char>& body)
{
  Cmd cmd = HttpCmdConverter::urlToCmd(http->getUrl(), CamModel::Hero5);
  
  if (code >= static_cast<C_HTTP::HttpResponseCode>(C_HTTP::ResponseCode::BadRequest))
    {
      // not successful
      d_owner->handleCommandFailed(this, cmd, GPError::Response);
      return;
    }

  // successful response => connected
  d_connected = true;
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
bool GoProHero5::handleTimeOut(const KERN::KernelTimer& timer)
{
  if (timer.is(d_timer_stream_check))
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
