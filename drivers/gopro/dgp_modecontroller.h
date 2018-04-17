#ifndef DGP_MODECONTROLLER_H
#define DGP_MODECONTROLLER_H

#include "dgp_gopro.h"

#include "core_owner.h"

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

    static std::string getReqStr(Req);
    
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(ModeController);
      virtual void handleFailedRequest(ModeController*,
				       Req,
				       const std::string&) = 0;

      // Internal state is not guaranteed, so delete on a zero timer in this callback.
      virtual void handleInternalFailure(ModeController*) = 0;
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

    SET_OWNER();

    void connect(); 
    void nextMode();
    void trigger();
    void startStream();
    void stopStream();

  private:
    //GoPro::Owner
    void handleCommandSuccessful(GoPro*, GoPro::Cmd) override;
    void handleCommandFailed(GoPro*,
			     GoPro::Cmd,
			     GoPro::Error,
			     const std::string&) override;
    void handleStreamDown(GoPro*) override;

  private:
    void startRequest(Req);

    void processNextReq();
    // This function will always result in calling a gopro command; it does not check if there is a request in process.
    void processReqWithLastSuccessfulCmd(GoPro::Cmd last_cmd);

    // For all of the following internal requests, providing a last command parameter of GoPro::Cmd::Unknown
    // will start the request from its first command.
    void internalConnect(GoPro::Cmd last_cmd); 
    void internalNextMode(GoPro::Cmd last_cmd);
    void internalTrigger(GoPro::Cmd last_cmd);
    void internalStartStream(GoPro::Cmd last_cmd);
    void internalStopStream(GoPro::Cmd last_cmd);
    
    void nextModeCmdsAfterStatus();
    
    void ownerInternalFailure();
    void ownerSuccessfulRequest();

    static std::string className() { static std::string name = "D_GP::ModeController::"; return name; }
    
  private:		
    CORE::Owner<Owner> d_owner;

    std::unique_ptr<GoPro> d_gp;
    std::list<Req> d_reqs;
  };
};
#endif
