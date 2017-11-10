#ifndef DGP_MODECONTROLLER_H
#define DGP_MODECONTROLLER_H

#include "dgp_gopro.h"

#include <list>
#include <memory>
#include <map>

namespace D_GP
{
  /*
    This class is simply used to change the camera mode, trigger, and start/stop a live stream.
  */
  
  class ModeController final : GoPro::Owner
  {
  public:
    enum class Req
    {
      Connect,
      NextMode,
      Trigger,
      StartStream,
      StopStream,
      Unknown
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
      virtual void handleFailedRequest(ModeController*, Req) = 0;
      virtual void handleSuccessfulRequest(ModeController*, Req) = 0;
    };
    
    struct CtrlParams
    {
      CtrlParams& setType(CamModel m) { model = m; return *this; }
      CtrlParams& setName(std::string s) { name = std::move(s); return *this; }
      
      CamModel model = CamModel::Hero5;
      std::string name = "ToBeNamed";
    };
    
  public:
    ModeController(Owner*, const CtrlParams&);
    void setOwner(Owner* o) { d_owner = o; }

    void connect(); 
    void nextMode();
    void trigger();
    void startStream();
    void stopStream();

  private:
    //GoPro::Owner
    void handleCommandSuccessful(GoPro*, GoPro::Cmd) override;
    void handleCommandFailed(GoPro*, GoPro::Cmd, GoPro::Error) override;
    void handleStreamDown(GoPro*) override;

  private:
    void processStatus();
    bool processNextMode();
    void sendFailedRequest();
    
  private:		
    Owner* d_owner = nullptr;

    std::unique_ptr<GoPro> d_gp;
    std::list<Req> d_reqs;
  };
};
#endif
