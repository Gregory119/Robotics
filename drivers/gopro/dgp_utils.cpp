#include "dgp_utils.h"

#include <cassert>

using namespace D_GP;

static constexpr char s_h5_ip_url[] = "http://10.5.5.9";
static constexpr char s_h5_bacpac[] = "/bacpac/cv";
static constexpr char s_h5_wifipair[] = "/gp/gpControl/command/wireless/pair/complete";
static constexpr char s_h5_mode[] = "/gp/gpControl/command/sub_mode?";
static constexpr char s_h5_shutter[] = "/gp/gpControl/command/shutter?p=";
static constexpr char s_h5_stream[] = "/gp/gpControl/execute?p1=gpStream&a1=proto_v2&c1=restart";

//----------------------------------------------------------------------//
std::string Utils::appendToAddress(const std::string& cmd,
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
std::string Utils::cmdToUrl(GoPro::Cmd cmd,
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
std::string Utils::cmdToUrl(GoPro::Cmd cmd,
			    CamModel model)
{
  std::vector<std::string> params; // empty
  return cmdToUrl(cmd, model, params);
}

//----------------------------------------------------------------------//
std::string
Utils::cmdToUrlHero5(GoPro::Cmd cmd,
		     const std::vector<std::string>& params)
{
  if (!validUrlParamsHero5(cmd, params))
    {
      assert(false);
      return "";
    }
  
  switch (cmd)
    {
    case GoPro::Cmd::Connect:
      return appendToAddress(s_h5_wifipair+"?success=1&deviceName="+params.at(0),
			     CamModel::Hero5);

    case GoPro::Cmd::Status:
      return appendToAddress("/gp/gpControl/status",
			     CamModel::Hero5);

    case GoPro::Cmd::SetModePhotoSingle:
      return appendToAddress(s_h5_mode+"mode=0&sub_mode=0",
			     CamModel::Hero5);
      
    case GoPro::Cmd::SetModePhotoContinuous:
      return appendToAddress(s_h5_mode+"mode=0&sub_mode=1",
			     CamModel::Hero5);
      
    case GoPro::Cmd::SetModePhotoNight:
      return appendToAddress(s_h5_mode+"mode=0&sub_mode=2",
			     CamModel::Hero5);
      
    case GoPro::Cmd::SetModeVideoNormal:
      return appendToAddress(s_h5_mode+"mode=1&sub_mode=0",
			     CamModel::Hero5);
      
    case GoPro::Cmd::SetModeVideoTimeLapse:
      return appendToAddress(s_h5_mode+"mode=1&sub_mode=1",
			     CamModel::Hero5);
      
    case GoPro::Cmd::SetModeVideoPlusPhoto:
      return appendToAddress(s_h5_mode+"mode=1&sub_mode=2",
			     CamModel::Hero5);
      
    case GoPro::Cmd::SetModeMultiShotBurst:
      return appendToAddress(s_h5_mode+"mode=2&sub_mode=0",
			     CamModel::Hero5);
      
    case GoPro::Cmd::SetModeMultiShotTimeLapse:
      return appendToAddress(s_h5_mode+"mode=2&sub_mode=1",
			     CamModel::Hero5);
      
    case GoPro::Cmd::SetModeMultiShotNightLapse:
      return appendToAddress(s_h5_mode+"mode=2&sub_mode=2",
			     CamModel::Hero5);
            
    case GoPro::Cmd::SetShutterTrigger:
      return appendToAddress(s_h5_shutter+"1",
			     CamModel::Hero5);
      
    case GoPro::Cmd::SetShutterStop:
      return appendToAddress(s_h5_shutter+"0",
			     CamModel::Hero5);
      
    case GoPro::Cmd::LiveStream:
      return appendToAddress(s_h5_stream,
			     CamModel::Hero5);

    case GoPro::Cmd::Unknown:
      assert(false);
      return "";
    };

  assert(false);
  return "";  
}

//----------------------------------------------------------------------//
bool Utils::validUrlParamsHero5(GoPro::Cmd cmd,
				const std::vector<std::string>& params)
{
  switch (cmd)
    {
    case GoPro::Cmd::Connect:
      if (params.size() != 1)
	{
	  assert(false);
	  return false;
	}
      return true;

    case GoPro::Cmd::Status:
    case GoPro::Cmd::SetModePhotoSingle:
    case GoPro::Cmd::SetModePhotoContinuous:
    case GoPro::Cmd::SetModePhotoNight:
    case GoPro::Cmd::SetModeVideoNormal:
    case GoPro::Cmd::SetModeVideoTimeLapse:
    case GoPro::Cmd::SetModeVideoPlusPhoto:
    case GoPro::Cmd::SetModeMultiShotBurst:
    case GoPro::Cmd::SetModeMultiShotTimeLapse:
    case GoPro::Cmd::SetModeMultiShotNightLapse:
    case GoPro::Cmd::SetShutterTrigger:
    case GoPro::Cmd::SetShutterStop:
    case GoPro::Cmd::LiveStream:
      if (!params.empty())
	{
	  assert(false);
	  return false;
	}
      return true;
      
    case GoPro::Cmd::Unknown:
      assert(false);
      return false;
    };

  assert(false);
  return "";  
}

//----------------------------------------------------------------------//
Mode Utils::extractModeStatus(int mode, int sub_mode)
{
  switch (mode)
    {
    case 0: // video
      switch (sub_mode)
	{
	case 0:
	  return Mode::VideoNormal;
	  
	case 1:
	  return Mode::VideoTimeLapse;

	case 2:
	  return Mode::VideoPlusPhoto;

	case 3:
	  return Mode::VideoLooping;
	}
      break;

    case 1: // photo
      switch (sub_mode)
	{
	case 0:
	  return Mode::PhotoSingle;
	  
	case 1:
	  return Mode::PhotoContinuous;

	case 2:
	  return Mode::PhotoNight;
	}
      break;

    case 2: // multishot
      switch (sub_mode)
	{
	case 0:
	  return Mode::MultiShotBurst;
	  
	case 1:
	  return Mode::MultiTimeLapse;

	case 2:
	  return Mode::MultiNightLapse;
	}
      break;
    }
  return Mode::Unknown;
}

