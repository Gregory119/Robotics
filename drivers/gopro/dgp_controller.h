#ifndef DGP_CONTROLLER_H
#define DGP_CONTROLLER_H

#include "dgp_gopro.h"
#include "kn_callbacktimer.h"

#include <memory>
#include <map>

namespace D_GP
{
  enum class GoProControllerReq
  {
    Unknown, 
    Connect,
    Photo,
    ToggleRecording,
  };

  enum class GPStateId
  {
    Disconnected,
    Connected,
    Photo,
    Video
  };
  
  class GPState;
  class StateConnected;
  class StateDisconnected;
  class StatePhoto;
  class StateVideo;
  class GoProController;
  class GoProControllerOwner
  {
  public:
    GoProControllerOwner() = default;
    virtual ~GoProControllerOwner() = default;
    GoProControllerOwner& operator=(const GoProControllerOwner&) = default;
    GoProControllerOwner(const GoProControllerOwner&) = default;
   
  private:
    friend GoProController;
    virtual void handleFailedRequest(GoProController*, GoProControllerReq) = 0;
    virtual void handleSuccessfulRequest(GoProController*, GoProControllerReq) = 0;
  };

  class GoProController final : GoProOwner
  {
  public:
    struct GPCtrlParams
    {
      GPCtrlParams& setType(CamModel m) { model = m; return *this; }
      GPCtrlParams& setName(std::string s) { name = std::move(s); return *this; }
      
      CamModel model = CamModel::Hero5;
      std::string name = "Robot";
    };
    
  public:
    GoProController(GoProControllerOwner*, const GPCtrlParams&);
    ~GoProController();

    // If this fails, it continuously tries to reconnect. Should consider setting the maximum number of retry attempts.
    void connectReq(); 
    void takePhotoReq();
    void toggleRecordingReq();
    
  private:
    //GoProOwner
    void handleCommandSuccessful(GoPro*, Cmd) override;
    void handleCommandFailed(GoPro*, Cmd, GPError) override;

  private:
    void setState(GPStateId);
    GoProControllerReq getRequest();
    void processCurrentState();
    bool isRecording() { return d_is_recording; }
    void toggleRecState() { d_is_recording = !d_is_recording; }
		
  private:
    friend GPState;
    friend StateConnected;
    friend StateDisconnected;
    friend StatePhoto;
    friend StateVideo;
		
    GoProControllerOwner* d_owner = nullptr;

    std::unique_ptr<GoPro> d_gp;
    bool d_is_recording = false;

    GPStateId d_state_id = GPStateId::Disconnected;
    std::map<GPStateId, std::unique_ptr<GPState>> d_states;
    GPState* d_state = nullptr;

    std::list<GoProControllerReq> d_reqs;

    KERN::CallbackTimer d_timer_recreate_gopro;
  };

  class GPState
  {
  public:
    virtual ~GPState() = default;
    virtual void process(GoProController&) = 0;
  };

  class StateConnected : public GPState
  {
    void process(GoProController&) override;
  };

  class StateDisconnected : public GPState
  {
    void process(GoProController&) override;
  };
	
  class StatePhoto : public GPState
  {
    void process(GoProController&) override;
  };

  class StateVideo : public GPState
  {
    void process(GoProController&) override;
  };

  class StateRecording : public GPState
  {
    void process(GoProController&) override;
  };
};
#endif
