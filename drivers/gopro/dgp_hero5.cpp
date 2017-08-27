#include "dgp_hero5.h"

#include "dgp_types.h"

#include "chttp_simple.h"

#include <cassert>

using namespace D_GP;

//----------------------------------------------------------------------//
GoProHero5::GoProHero5(GoProOwner* o)
  : GoPro(o)
{
  assert(o != nullptr);
}

//----------------------------------------------------------------------//
void GoProHero5::connectWithName(const std::string& name)
{
  assert(!name.empty());
  stop();  

  d_http.reset(new C_HTTP::HttpOperations(d_owner));
  
  if (!d_http->init())
    {
      d_owner->handleFailedCommand(this,Cmd::Connect);
      return;
    }
  
  if (!d_http->get(CreateCmdUrl(g_bacpac)))
    {
      d_owner->handleFailedCommand(this,Cmd::Connect);
      return;
    }
}

//----------------------------------------------------------------------//
void GoProHero5::setMode(Mode mode)
{
  std::string params;
  switch (mode)
    {
    case Mode::Photo:
      params = "?p=1";
      break;
      
    case Mode::Video:
      params = "?p=0";
      break;

    default:
      assert(false);
      return;
      break;
    }
  
  if (!d_http->post(CreateCmdUrl(g_mode)+params))
    {
      d_owner->handleFailedCommand(this,Cmd::SetMode);
      return;
    }
  d_owner->handleModeSet(this,mode);
}

//----------------------------------------------------------------------//
void GoProHero5::setShutter(bool state)
{
  std::string params;
  if (state)
    {
      params = "?p=1";
    }
  else
    {
      params = "?p=0";
    }
  
  if(!d_http->post(CreateCmdUrl(g_shutter)+params))
    {
      d_owner->handleFailedCommand(this,Cmd::SetShutter);
      return;
    }
  d_owner->handleShutterSet(this, state);
}

//----------------------------------------------------------------------//
void GoProHero5::startLiveStream()
{
  // possibly wait on state of pi analogue connection
  // send stream request command on a timer
  // start a terminal process of omxplayer to send udp stream to the pi analogue output
}

//----------------------------------------------------------------------//
void GoProHero5::stopLiveStream()
{
  // stop waiting on state of pi analogue connection
  // stop requesting stream on a timer
  // stop the terminal process of omxplayer
}

//----------------------------------------------------------------------//
void GoProHero5::stop()
{
  d_timer_connect.disable();
  stopLiveStream();
}

//----------------------------------------------------------------------//
void GoProHero5::handleFailed(HttpOperations* o,
			      C_HTTP::HttpOpError error)
{
  // sent command was unsuccessful
  // http->getUrl()

  // --if connect bacpac command--
  d_owner->handleFailedCommand(this,Cmd::Connect);
  
  // if connect command and connected =>failed to connect
  // if connect command and disconnected =>failed to connect once off (connect timer not running)
  d_owner->handleFailedCommand(this,Cmd::Connect);
}

//----------------------------------------------------------------------//
void GoProHero5::handleResponse(HttpOperations* http,
				C_HTTP::HttpResponseCode,
				const std::vector<std::string>& headers,
				const std::vector<char>& body)
{
  // sent command was successful
  
  // http->getUrl()
  // parse the url to determine the successful command sent
  // call owner handle command

  
  // shutter set??



  // mode set??
  
  
  
  // --if connect bacpac command--
  if (!d_http->get(CreateCmdUrl(g_wifipair+std::string("?success=1&deviceName=")+name)))
    {
      d_owner->handleFailedCommand(this,Cmd::Connect);
      return;
    }

  // --if connect name command--
  // start a timer to continuously check the connection status
  // d_timer_connect.restartTimerMs(5000);  
  // if not connected =>handleConnectionUp
  // if connected =>nothing
}

//----------------------------------------------------------------------//
//GoProHero5::
