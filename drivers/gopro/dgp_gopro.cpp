#include "dgp_gopro.h"

#include <cassert>

using namespace D_GP;

//----------------------------------------------------------------------//
std::string GoPro::getCmdStr(Cmd cmd)
{
  switch (cmd)
    {
    case Cmd::Connect:
      return "Connect";
      
    case Cmd::Status:
      return "Status";
      
    case Cmd::SetModePhotoSingle:
      return "SetModePhotoSingle";
      
    case Cmd::SetModePhotoContinuous:
      return "SetModePhotoContinuous";
      
    case Cmd::SetModePhotoNight:
      return "SetModePhotoNight";
      
    case Cmd::SetModeVideoNormal:
      return "SetModeVideoNormal";
      
    case Cmd::SetModeVideoTimeLapse:
      return "SetModeVideoTimeLapse";
      
    case Cmd::SetModeVideoPlusPhoto:
      return "SetModeVideoPlusPhoto";
      
    case Cmd::SetModeVideoLooping:
      return "SetModeVideoLooping";
      
    case Cmd::SetModeMultiShotBurst:
      return "SetModeMultiShotBurst";
      
    case Cmd::SetModeMultiShotTimeLapse:
      return "SetModeMultiShotTimeLapse";
      
    case Cmd::SetModeMultiShotNightLapse:
      return "SetModeMultiShotNightLapse";
      
    case Cmd::SetShutterTrigger:
      return "SetShutterTrigger";
      
    case Cmd::SetShutterStop:
      return "SetShutterStop";
      
    case Cmd::StartLiveStream:
      return "StartLiveStream";
      
    case Cmd::StopLiveStream:
      return "StopLiveStream";
      
    case Cmd::SetBitRate:
      return "SetBitRate";
      
    case Cmd::Unknown:
      assert(false);
      return std::string();
    }
  assert(false);
  return std::string();
}

//----------------------------------------------------------------------//
GoPro::GoPro(GoPro::Owner* o)
  : d_owner(o)
{}

//----------------------------------------------------------------------//
void GoPro::ownerCommandFailed(GoPro::Cmd cmd,
			       GoPro::Error err,
			       const std::string& msg)
{
  d_owner.call(&Owner::handleCommandFailed, this, cmd, err, msg);
}

//----------------------------------------------------------------------//
void GoPro::ownerCommandSuccessful(GoPro::Cmd cmd)
{
  d_owner.call(&Owner::handleCommandSuccessful, this, cmd);
}

//----------------------------------------------------------------------//
void GoPro::ownerStreamDown()
{
  d_owner.call(&Owner::handleStreamDown, this);
}
