#ifndef DGP_FASTCONTROLLER_H
#define DGP_FASTCONTROLLER_H

#include "dgp_gopro.h"
#include "kn_callbacktimer.h"

#include <list>
#include <memory>
#include <map>

namespace D_GP
{
  /*
    This class is simply used to change the camera mode, trigger, and start/stop a live stream. It assumes that the camera is already paired with the device.

    TODO: 
    - For reliable usage, this class still needs to check the state of the camera before performing a command (consider recording status). 
    - There needs to be a pairing process in which the connect command is sent. This can be done with a bluetooth based app that sets the wifi password. 
  */
  
  class ModeController final : GoProOwner
  {
  public:
    enum class Req
    {
      Unknown,
      Connect,
      NextMode,
      Trigger,
      StartStream,
      StopStream
    };

    class Owner
    {
    public:
      Owner& operator=(const Owner&) = delete;
      Owner(const Owner&) = delete;
      Owner(Owner&&) = delete;
      Owner& operator=(Owner&&) = delete;

    protected:
      Owner() = default; // enforce inheritance
      ~Owner() = default; // only deleteable by child class
      
    private:
      friend ModeController;
      virtual void handleFailedRequest(FastController*, Req) = 0;
      virtual void handleSuccessfulRequest(FastController*, Req) = 0;
    };
    
  public:
    struct CtrlParams
    {
      CtrlParams& setType(CamModel m) { model = m; return *this; }
      CtrlParams& setName(std::string s) { name = std::move(s); return *this; }
      
      CamModel model = CamModel::Hero5;
      std::string name = "ToBeNamed";
    };
    
  public:
    ModeController(Owner*, const CtrlParams&);

    void connectReq(); 
    void nextMode();
    void trigger();
    void startLive();
    void stopLive();

  private:
    //GoProOwner
    void handleCommandSuccessful(GoPro*, Cmd) override;
    void handleCommandFailed(GoPro*, Cmd, GPError) override;

  private:		
    Owner* d_owner = nullptr;

    std::unique_ptr<GoPro> d_gp;
    std::list<Req> d_reqs;
  };
};
#endif
