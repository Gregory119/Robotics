#include "dgp_simplegopro.h"

#include "dgp_types.h"

#include "chttp_simple.h"

#include <cassert>

using namespace D_GP;

//----------------------------------------------------------------------//
SimpleGoPro::SimpleGoPro(GoProOwner* o)
  : GoPro(o),
    d_http_post(new C_HTTP::SimpleHttpPost())
{}

//----------------------------------------------------------------------//
void SimpleGoPro::connectWithName(const std::string& name)
{
  assert(!name.empty());
  
  if (!d_http_post->isReady())
    {
      d_http_post->init();
    }
  
  if (!d_http_post->isReady())
    {
      owner()->handleFailedCommand(this,Cmd::Connect);
      return;
    }
  
  if (!d_http_post->post(CreateCmdUrl(g_bacpac)))
    {
      owner()->handleFailedCommand(this,Cmd::Connect);
      return;
    }

  if (!d_http_post->post(CreateCmdUrl(g_wifipair+std::string("?success=1&deviceName=")+name)))
    {
      owner()->handleFailedCommand(this,Cmd::Connect);
      return;
    }
    
  setConnected();
}

//----------------------------------------------------------------------//
void SimpleGoPro::setMode(Mode mode)
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
  
  if (!d_http_post->post(CreateCmdUrl(g_mode)+params))
    {
      owner()->handleFailedCommand(this,Cmd::SetMode);
      return;
    }
  owner()->handleModeSet(this,mode);
}

//----------------------------------------------------------------------//
void SimpleGoPro::setShutter(bool state)
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
  
  if(!d_http_post->post(CreateCmdUrl(g_shutter)+params))
    {
      owner()->handleFailedCommand(this,Cmd::SetShutter);
      return;
    }
  owner()->handleShutterSet(this, state);
}

//----------------------------------------------------------------------//
//SimpleGoPro::
