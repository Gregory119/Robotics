#include "dgp_utils.h"

#include <cassert>

using namespace D_GP;

static const std::string s_h5_ip_url = "http://10.5.5.9";
static const std::string s_h5_bacpac = "/bacpac/cv";
static const std::string s_h5_wifipair = "/gp/gpControl/command/wireless/pair/complete";
static const std::string s_h5_mode = "/gp/gpControl/command/mode?p=";
static const std::string s_h5_shutter = "/gp/gpControl/command/shutter?p=";
static const std::string s_h5_stream = "/gp/gpControl/execute?p1=gpStream&a1=proto_v2&c1=restart";
static const std::string s_h5_param_photo_mode = "1";
static const std::string s_h5_param_video_mode = "0";
static const std::string s_h5_param_multishot_mode = "2";
static const std::string s_h5_param_shutter_trigger = "1";
static const std::string s_h5_param_shutter_stop = "0";
static const std::string s_h5_status = "/gp/gpControl/status";

//----------------------------------------------------------------------//
std::string CmdToUrlConverter::appendToAddress(const std::string& cmd,
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
std::string CmdToUrlConverter::cmdToUrl(Cmd cmd,
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
std::string CmdToUrlConverter::cmdToUrl(Cmd cmd,
					CamModel model)
{
  std::vector<std::string> params; // empty
  return cmdToUrl(cmd, model, params);
}

//----------------------------------------------------------------------//
std::string
CmdToUrlConverter::cmdToUrlHero5(Cmd cmd,
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
      return appendToAddress(s_h5_wifipair+"?success=1&deviceName="+params.at(0),
			     CamModel::Hero5);

    case Cmd::Status:
      return appendToAddress(s_h5_status,
			     CamModel::Hero5);
      
    case Cmd::SetModePhoto:
      return appendToAddress(s_h5_mode+s_h5_param_photo_mode,
			     CamModel::Hero5);

    case Cmd::SetModeVideo:
      return appendToAddress(s_h5_mode+s_h5_param_video_mode,
			     CamModel::Hero5);

    case Cmd::SetModeMultiShot:
      return appendToAddress(s_h5_mode+s_h5_param_multishot_mode,
			     CamModel::Hero5);
      
    case Cmd::SetShutterTrigger:
      return appendToAddress(s_h5_shutter+s_h5_param_shutter_trigger,
			     CamModel::Hero5);
      
    case Cmd::SetShutterStop:
      return appendToAddress(s_h5_shutter+s_h5_param_shutter_stop,
			     CamModel::Hero5);
      
    case Cmd::LiveStream:
      return appendToAddress(s_h5_stream,
			     CamModel::Hero5);

    case Cmd::Unknown:
      assert(false);
      return "";
    };

  assert(false);
  return "";  
}

//----------------------------------------------------------------------//
Cmd CmdToUrlConverter::urlToCmd(const std::string& url, CamModel model)
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
Cmd CmdToUrlConverter::urlToCmdHero5(const std::string& url)
{
  std::string tmp = appendToAddress(s_h5_wifipair+"?success=1&deviceName=",
				    CamModel::Hero5);
  if (url.find(tmp) != std::string::npos)
    {
      return Cmd::Connect;
    }

  if (url == appendToAddress(s_h5_mode+s_h5_param_photo_mode,
			     CamModel::Hero5))
    {
      return Cmd::SetModePhoto;
    }

  if (url == appendToAddress(s_h5_mode+s_h5_param_video_mode,
			     CamModel::Hero5))
    {
      return Cmd::SetModeVideo;
    }

  if (url == appendToAddress(s_h5_mode+s_h5_param_multishot_mode,
			     CamModel::Hero5))
    {
      return Cmd::SetModeVideo;
    }
      
  if (url == appendToAddress(s_h5_shutter+s_h5_param_shutter_trigger,
			     CamModel::Hero5))
    {
      return Cmd::SetShutterTrigger;
    }

  if (url == appendToAddress(s_h5_shutter+s_h5_param_shutter_stop,
			     CamModel::Hero5))
    {
      return Cmd::SetShutterStop;
    }

  if (url == appendToAddress(s_h5_stream,
			     CamModel::Hero5))
    {
      return Cmd::LiveStream;
    }

  assert(false);
  return Cmd::Unknown;
}

//----------------------------------------------------------------------//
bool CmdToUrlConverter::validUrlParamsHero5(Cmd cmd,
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

//----------------------------------------------------------------------//
Mode CmdToUrlConverter::extractModeStatus(const std::string& mode_status)
{
  if (mode_status == s_h5_param_photo_mode)
    {
      return Mode::Photo;
    }

  if (mode_status == s_h5_param_video_mode)
    {
      return Mode::Video;
    }

  if (mode_status == s_h5_param_multishot_mode)
    {
      return Mode::MultiShot;
    }
}

/*
//----------------------------------------------------------------------//
std::string CmdToUrlConverter::cmdToMessage(Cmd cmd)
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
