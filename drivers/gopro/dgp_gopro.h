#ifndef DGP_GOPRO_H
#define DGP_GOPRO_H

#include "chttp_simple.h"
#include "dgp_types.h"

/*
  This is the gopro interface.

  First connect to go pro before calling other commands. Verify connection with isConnected();
*/

namespace D_GP
{
  class GoPro;
  class GoProOwner // inherit privately
  {
  public:
    GoProOwner& operator=(const GoProOwner&) = default;
    GoProOwner(const GoProOwner&) = default;

    virtual void handleFailedCommand(GoPro*, Cmd) = 0;
    virtual void handleShutterSet(GoPro*, bool) = 0;
    virtual void handleModeSet(GoPro*, Mode) = 0;
    virtual void handleConnectionUp(GoPro*) = 0;
    
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
    
  protected:
    //only to be inherited
    explicit GoPro(GoProOwner* o);
    GoProOwner* d_owner = nullptr;
  };
};

#endif
