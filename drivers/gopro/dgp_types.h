#ifndef DGP_TYPES_H
#define DGP_TYPES_H

namespace D_GP
{
  enum class CamModel
  {
    Hero5
  };
  
  enum class Cmd
  { 
    Connect,
    Status,
    SetModePhoto,
    SetModeVideo,
    SetModeMultiShot,
    SetShutterTrigger,
    SetShutterStop,
    LiveStream,
    Unknown
  };

  enum class Mode
  {
    Unknown,
    Photo,
    Video,
    MultiShot
  };

  struct Status
  {
    Mode mode = Mode::Unknown;
    bool is_recording = false;
    bool is_streaming = false;
  };
};

#endif
