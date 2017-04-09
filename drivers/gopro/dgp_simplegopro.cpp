#include "dgp_simplegopro.h"

#include "dgp_types.h"

#include "chttp_simplepost.h"

using namespace D_GP;

//----------------------------------------------------------------------//
SimpleGoPro::SimpleGoPro(GoProOwner* o)
  : GoPro(o),
    d_http_post(new C_HTTP::SimpleHttpPost())
{}

//----------------------------------------------------------------------//
void SimpleGoPro::connectWithName(std::string name)
{
  if (!d_http_post->isReady())
    {
      d_http_post->init();
    }
  
  if (!d_http_post->isReady())
    {
      return;
    }
  
  if (!d_http_post->post(CreateCmdUrl(g_bacpac)))
    {
      return;
    }
  
  if (!d_http_post->post(CreateCmdUrl(g_wifipair)))
    {
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
      
    case Mode::MultiShot:
      params = "?p=2";;
      break;
    }
  
  if (!d_http_post->post(CreateCmdUrl(g_mode)+params))
    {
      owner()->handleFailedCommand(this,Cmd::SetMode);
      return;
    }
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
  
  if(d_http_post->post(CreateCmdUrl(g_shutter)+params))
    {
      owner()->handleFailedCommand(this,Cmd::SetShutter);
      return;
    }
  owner()->handleShutterSet(this, state);
}

//----------------------------------------------------------------------//
//SimpleGoPro::
