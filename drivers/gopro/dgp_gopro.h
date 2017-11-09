#ifndef DGP_GOPRO_H
#define DGP_GOPRO_H

#include "dgp_types.h"

/*
  This is the gopro interface.

  - The concrete class should automatically attempt to connect after construction (using zero timer). 
  - The GoPro should automatically and continuously attempt to reconnect after a successful connection.
*/

#include <string>

namespace D_GP
{
  class GoPro
  {
  public:
    enum class Error
    {
      Internal, // internal to the client of the gopro (this should not happen)
      Response, // gopro responded with error message
      ResponseData, // gopro response data was unexpected
      Timeout // a timeout occured after waiting for a gopro response
    };
    
    enum class Cmd
    { 
      Connect,
      Status,
      SetModePhotoSingle,
      SetModePhotoContinuous,
      SetModePhotoNight,
      SetModeVideoNormal,
      SetModeVideoTimeLapse,
      SetModeVideoPlusPhoto,
      SetModeVideoLooping,
      SetModeMultiShotBurst,
      SetModeMultiShotTimeLapse,
      SetModeMultiShotNightLapse,
      SetShutterTrigger,
      SetShutterStop,
      StartLiveStream,
      StopLiveStream,
      SetBitRate,
      Unknown
    };

    class Owner // inherit privately
    {
    public:
      Owner(const Owner&) = delete;
      Owner& operator=(const Owner&) = delete;
      Owner(Owner&&) = delete;
      Owner&& operator=(Owner&&) = delete;

      // queued commands should be cleared on a failure
      virtual void handleCommandFailed(GoPro*, GoPro::Cmd, GoPro::Error) = 0;
      virtual void handleCommandSuccessful(GoPro*, GoPro::Cmd) = 0;
    
    protected:
      Owner() = default;
      ~Owner() = default;
    };
    
  public:
    virtual ~GoPro() = default;
    GoPro& operator=(const GoPro&) = default;
    GoPro(const GoPro&) = default;
    // move constructor & operator

    virtual void connect() = 0;
    virtual void status() = 0; 
    virtual void setMode(Mode) = 0;
    virtual void setShutter(bool) = 0;
    virtual void setBitRatePerSecond(unsigned) = 0;
    virtual void startLiveStream() = 0;
    virtual void stopLiveStream() = 0;
    virtual bool hasBufferedReqs() = 0;
    virtual void cancelBufferedCmds() = 0;
    // Can add additional virtual functions which have asserts in their definition. The GoPro that supports the command will have it defined.
    
    const Status& getStatus() { return d_status; }
    
  protected:
    //only to be inherited
    explicit GoPro(Owner* o);
    Owner* d_owner = nullptr;

    Status d_status;
  };
};

#endif
