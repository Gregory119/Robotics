#include "dgp_types.h"

#include <cassert>

using namespace D_GP;

namespace D_GP
{
  const std::string& g_ip_url = "http://10.5.5.9";
  const std::string& g_bacpac = "/bacpac/cv";
  const std::string& g_wifipair = "/gp/gpControl/command/wireless/pair/complete";
  const std::string& g_mode = "/gp/gpControl/command/mode";
  const std::string& g_shutter = "/gp/gpControl/command/shutter";
};

//----------------------------------------------------------------------//
std::string D_GP::CreateCmdUrl(const std::string& cmd)
{
  return g_ip_url + cmd;
}

//----------------------------------------------------------------------//
std::string D_GP::cmdToString(Cmd cmd)
{
  std::string ret;
  switch (cmd)
    {
    case Cmd::Connect:
      ret = "Connecting";
      break;
      
    case Cmd::SetMode:
      ret = "Setting the mode";
      break;
      
    case Cmd::SetShutter:
      ret = "Setting the shutter";
      break;

    default:
      assert(false);
      return ret;
    };
  return ret;
}

//----------------------------------------------------------------------//
std::string D_GP::reqToString(Request req)
{
  std::string ret;
  switch (req)
    {
    case Request::Connect:
      ret = "Connecting";
      break;
      
    case Request::Picture:
      ret = "Taking a picture";
      break;
      
    case Request::MultiShot:
      ret = "Taking a multiShot";
      break;
      
    case Request::StartStopRec:
      ret = "Starting or stopping a recording";
      break;
      
    default:
      assert(false);
      return ret;
    };
  return ret;
}
