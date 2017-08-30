#ifndef DGP_GOPRO_H
#define DGP_GOPRO_H

#include "dgp_types.h"

/*
  This is the gopro interface.

  - First connect to go pro before calling other commands. 
  - The GoPro should automatically and continuously attempt to reconnect after a successful connection.
*/

namespace D_GP
{
  class GoPro;
  class GoProOwner // inherit privately
  {
  public:
    GoProOwner& operator=(const GoProOwner&) = default;
    GoProOwner(const GoProOwner&) = default;

    virtual void handleCommandFailed(GoPro*, Cmd) = 0; // eg. any failure other than a timeout
    virtual void handleCommandSuccessful(GoPro*,
					 Cmd) = 0;
    virtual void handleDisconnected(GoPro*, Cmd) = 0; // eg. timeout on waiting for message response
    
  protected:
    GoProOwner() = default;
    ~GoProOwner() = default;
  };
  
  class GoPro
  {
  public:
    virtual ~GoPro() = default;
    GoPro& operator=(const GoPro&) = default;
    GoPro(const GoPro&) = default;

    virtual void connectWithName(const std::string&) = 0;
    virtual void setMode(Mode) = 0;
    virtual void setShutter(bool) = 0;
    virtual void startLiveStream() = 0;
    virtual void stopLiveStream() = 0;
    virtual bool isConnected() = 0;
    
  protected:
    //only to be inherited
    explicit GoPro(GoProOwner* o);
    GoProOwner* d_owner = nullptr;
  };
};

#endif
