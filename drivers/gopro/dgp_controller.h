#ifndef DGP_CONTROLLER_H
#define DGP_CONTROLLER_H

#include "dgp_gopro.h"
//#include "kn_timer.h" //implement this

#include <memory>
#include <map>

namespace D_GP
{
  enum class GPStateId
    {
      Disconnected,
      Connected,
      Photo,
      StartStopRec
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
    // the synchronous type controller will have undefined behaviour if it is not deleted after signaling a failure
    virtual void handleFailedRequest(GoProController*) = 0; 
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
    GoProController(GoProControllerOwner*, GPCtrlParams);
    ~GoProController();

    void connect();
    void takePhoto();
    void StartStopRecording();
    
  private:
    //GoPro
    void handleModeSet(GoPro*, Mode mode) override { d_mode = mode; }
    void handleShutterSet(GoPro*, bool) override {}
    void handleFailedCommand(GoPro*, Cmd) override;

  private:
    void setState(GPStateId);
    GPStateId getPrevState() { return d_prev_state_id; }
    void processCurrentState();
    void toggleRecording(); // sets to video mode
    void setMode(Mode); // set mode if different
		
  private:
    friend GPState;
    friend StateConnected;
    friend StateDisconnected;
    friend StatePhoto;
    friend StateVideo;
		
    GoProControllerOwner* d_owner = nullptr;

    std::unique_ptr<GoPro> d_gp;
    bool d_is_recording = false;
    std::string d_connect_name;
    Mode d_mode = Mode::Unknown;

    GPStateId d_state_id = GPStateId::Disconnected;
    GPStateId d_prev_state_id = GPStateId::Disconnected;
    std::map<GPStateId, std::unique_ptr<GPState>> d_states;
    GPState* d_state = nullptr;
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
};
#endif
