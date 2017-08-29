#ifndef DGP_TYPES_H
#define DGP_TYPES_H

namespace D_GP
{
  enum class CamModel
  {
    Hero5
  };
  
  enum class Cmd
  { // H5 implies hero 5
    Connect,
    SetModePhoto,
    SetModeVideo,
    SetShutterTrigger,
    SetShutterStop,
    LiveStream,
    Unknown
  };

  enum class Mode
  {
    Unknown,
    Photo,
    Video
  };
};

#endif
