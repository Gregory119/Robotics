#include "dgp_utils.h"

#include <cassert>

using namespace D_GP;

const std::string s_h5_ip_url = "http://10.5.5.9";
const std::string s_h5_bacpac = "/bacpac/cv";
const std::string s_h5_wifipair = "/gp/gpControl/command/wireless/pair/complete";
const std::string s_h5_mode = "/gp/gpControl/command/mode";
const std::string s_h5_shutter = "/gp/gpControl/command/shutter";
const std::string s_h5_stream = "/gp/gpControl/execute?p1=gpStream&a1=proto_v2&c1=restart";

//----------------------------------------------------------------------//
std::string CmdConverter::createCmdUrl(const std::string& cmd,
				       CamModel model)
{
  switch (model)
    {
    case CamModel::Hero5:
      return s_h5_ip_url + cmd;
    }

  assert(false);
  return "";
}

//----------------------------------------------------------------------//
std::string CmdConverter::cmdToUrl(Cmd cmd, CamModel model)
{
  switch (model)
    {
    case CamModel::Hero5:
      return cmdToUrlHero5(cmd);
    }
  
  assert(false);
  return "";
}

//----------------------------------------------------------------------//
std::string CmdConverter::cmdToUrlHero5(Cmd cmd)
{
  switch (cmd)
    {
    case Cmd::Connect:
      return createCmdUrl(s_h5_wifipair+"?success=1&deviceName=BushBot");
      
    case Cmd::SetModePhoto:
      return createCmdUrl(s_h5_mode+"?p=1");

    case Cmd::SetModeVideo:
      return createCmdUrl(s_h5_mode+"?p=0");

    case Cmd::SetShutterTrigger:
      return createCmdUrl(s_h5_shutter+"?p=1");
      
    case Cmd::SetShutterStop:
      return createCmdUrl(s_h5_shutter+"?p=0");
      
    case Cmd::LiveStream:
      return createCmdUrl(s_h5_stream);

    case Cmd::Unknown:
      assert(false);
      return "";
    };

  assert(false);
  return "";  
}

//----------------------------------------------------------------------//
Cmd CmdConverter::urlToCmd(const std::string& url, CamModel model)
{
  switch (model)
    {
    case CamModel::Hero5:
      return urlToCmdHero5(url);
    }
  assert(false);
  return Cmd::Unknown;
}

//----------------------------------------------------------------------//
Cmd CmdConverter::urlToCmdHero5(const std::string& url)
{
  if (url == createCmdUrl(s_h5_wifipair+"?success=1&deviceName=BushBot"))
    {
      return Cmd::Connect;
    }

  if (url == createCmdUrl(s_h5_mode+"?p=1"))
    {
      return Cmd::SetModePhoto;
    }

  if (url == createCmdUrl(s_h5_mode+"?p=0"))
    {
      return Cmd::SetModeVideo;
    }

  if (url == createCmdUrl(s_h5_shutter+"?p=1"))
    {
      return Cmd::SetShutterTrigger;
    }

  if (url == createCmdUrl(s_h5_shutter+"?p=0"))
    {
      return Cmd::SetShutterStop;
    }

  if (url == createCmdUrl(s_h5_stream))
    {
      return Cmd::LiveStream;
    }

  assert(false);
  return Cmd::Unknown;
}

/*
//----------------------------------------------------------------------//
std::string CmdConverter::cmdToMessage(Cmd cmd)
{
  switch (cmd)
    {
    case Cmd::Connect:
      return "Connecting";
      
    case Cmd::SetMode:
      return "Setting the mode";
      break;
      
    case Cmd::SetShutter:
      return "Setting the shutter";
      break;
    };
  assert(false);
  return "";
}
*/
