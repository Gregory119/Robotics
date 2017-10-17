#ifndef DGP_TYPES_H
#define DGP_TYPES_H

#include <string>

namespace D_GP
{
  enum class CamModel
  {
    Hero5,
    Unknown
  };
  
  enum class Mode
  {
    VideoNormal,
    VideoTimeLapse,
    VideoPlusPhoto,
    VideoLooping,
    PhotoSingle, // Seems to be discontinued
    PhotoContinuous,
    PhotoNight,
    MultiShotBurst,
    MultiTimeLapse,
    MultiNightLapse,
    Unknown
  };
  //video => photo => burst(multishot) => timelapse (multishot) => video - this sequence could be customised with the app

  struct Status
  {
    void clear() { *this = Status(); }
    bool loadStr(const std::string&, CamModel);
    void print();
    
    Mode d_mode = Mode::Unknown;
    bool d_is_recording = false;
    bool d_is_streaming = false;

  private:
    bool loadStrHero5(const std::string&);
  };
};

#endif
