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
std::string HttpCmdConverter::prependAddress(const std::string& cmd,
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
std::string HttpCmdConverter::cmdToUrl(Cmd cmd,
				       CamModel model,
				       const std::vector<std::string>& params)
{
  switch (model)
    {
    case CamModel::Hero5:
      return cmdToUrlHero5(cmd, params);
    }
  
  assert(false);
  return "";
}

//----------------------------------------------------------------------//
std::string HttpCmdConverter::cmdToUrl(Cmd cmd,
				       CamModel model)
{
  std::vector<std::string> params; // empty
  return cmdToUrl(cmd, model, params);
}

//----------------------------------------------------------------------//
std::string
HttpCmdConverter::cmdToUrlHero5(Cmd cmd,
				const std::vector<std::string>& params)
{
  if (!validUrlParamsHero5(cmd, params))
    {
      assert(false);
      return "";
    }
  
  switch (cmd)
    {
    case Cmd::Connect:
      return prependAddress(s_h5_wifipair+"?success=1&deviceName="+params.at(0),
			    CamModel::Hero5);
      
    case Cmd::SetModePhoto:
      return prependAddress(s_h5_mode+"?p=1",
			    CamModel::Hero5);

    case Cmd::SetModeVideo:
      return prependAddress(s_h5_mode+"?p=0",
			    CamModel::Hero5);

    case Cmd::SetShutterTrigger:
      return prependAddress(s_h5_shutter+"?p=1",
			    CamModel::Hero5);
      
    case Cmd::SetShutterStop:
      return prependAddress(s_h5_shutter+"?p=0",
			    CamModel::Hero5);
      
    case Cmd::LiveStream:
      return prependAddress(s_h5_stream,
			    CamModel::Hero5);

    case Cmd::Unknown:
      assert(false);
      return "";
    };

  assert(false);
  return "";  
}

//----------------------------------------------------------------------//
Cmd HttpCmdConverter::urlToCmd(const std::string& url, CamModel model)
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
Cmd HttpCmdConverter::urlToCmdHero5(const std::string& url)
{
  std::string tmp = prependAddress(s_h5_wifipair+"?success=1&deviceName=",
				   CamModel::Hero5);
  if (url.find(tmp) != std::string::npos)
    {
      return Cmd::Connect;
    }

  if (url == prependAddress(s_h5_mode+"?p=1",
			    CamModel::Hero5))
    {
      return Cmd::SetModePhoto;
    }

  if (url == prependAddress(s_h5_mode+"?p=0",
			    CamModel::Hero5))
    {
      return Cmd::SetModeVideo;
    }

  if (url == prependAddress(s_h5_shutter+"?p=1",
			    CamModel::Hero5))
    {
      return Cmd::SetShutterTrigger;
    }

  if (url == prependAddress(s_h5_shutter+"?p=0",
			    CamModel::Hero5))
    {
      return Cmd::SetShutterStop;
    }

  if (url == prependAddress(s_h5_stream,
			    CamModel::Hero5))
    {
      return Cmd::LiveStream;
    }

  assert(false);
  return Cmd::Unknown;
}

//----------------------------------------------------------------------//
bool HttpCmdConverter::validUrlParamsHero5(Cmd cmd,
					   const std::vector<std::string>& params)
{
  switch (cmd)
    {
    case Cmd::Connect:
      if (params.size() != 1)
	{
	  assert(false);
	  return false;
	}
      return true;
      
    case Cmd::SetModePhoto:
    case Cmd::SetModeVideo:
    case Cmd::SetShutterTrigger:
    case Cmd::SetShutterStop:
    case Cmd::LiveStream:
      if (!params.empty())
	{
	  assert(false);
	  return false;
	}
      return true;
      
    case Cmd::Unknown:
      assert(false);
      return false;
    };

  assert(false);
  return "";  
}

/*
//----------------------------------------------------------------------//
std::string HttpCmdConverter::cmdToMessage(Cmd cmd)
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
