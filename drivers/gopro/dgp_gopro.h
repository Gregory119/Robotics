#ifndef DGP_GOPRO_H
#define DGP_GOPRO_H

#include "chttp_simplepost.h"
#include "dgp_types.h"

/*
  This is the gopro interface.

  First connect to go pro before calling other commands. Verify connection with isConnected();
*/

namespace D_GP
{
  class GoPro;
  class SimpleGoPro;
  class GoProOwner
  {
  public:
    virtual ~GoProOwner();
    GoProOwner& operator=(const GoProOwner&) = default;
    GoProOwner(const GoProOwner&) = default;

  protected:
    GoProOwner() = default;
    
  private:
    friend GoPro;
    friend SimpleGoPro;
    virtual void handleFailedCommand(GoPro*, Cmd) = 0;
    virtual void handleShutterSet(GoPro*, bool) = 0;
    virtual void handleModeChanged(GoPro*, Mode) = 0;
  };
  
  class GoPro
  {
  public:
    virtual ~GoPro() = default;
    GoPro& operator=(const GoPro&) = default;
    GoPro(const GoPro&) = default;

    virtual void connectWithName(std::string) = 0;
    virtual void setMode(Mode) = 0;
    virtual void setShutter(bool) = 0;
    bool isConnected() { return d_is_connected; }
    
  protected: //only to be inherited
    explicit GoPro(GoProOwner* o);

    GoProOwner* owner() { return d_owner; }
    void setConnected() { d_is_connected=true; }

  private:
    GoProOwner* d_owner = nullptr;
    bool d_is_connected = false;
  };
};

#endif
