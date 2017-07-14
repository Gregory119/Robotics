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
      Connect,
      Photo,
      StartStopRec
    };
  
  class GPState;
  class StateConnect;
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
      GPCtrlParams& setType(ControlType t) { type = t; return *this; }
      GPCtrlParams& setName(std::string s) { name = std::move(s); return *this; }
      
      ControlType type = ControlType::Simple;
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
    void handleModeSet(GoPro*, Mode) override {}
    void handleShutterSet(GoPro*, bool) override {}
    void handleFailedCommand(GoPro*, Cmd) override;

  private:
    void setState(GPStateId);
    GPStateId getPrevState() { return d_prev_state_id; }
    void processState();
		
  private:
    friend GPState;
    friend StateConnect;
    friend StatePhoto;
    friend StateVideo;
		
    GoProControllerOwner* d_owner = nullptr;

    std::unique_ptr<GoPro> d_gp;
    bool d_is_recording = false;
    std::string d_connect_name;

    GPStateId d_state_id = GPStateId::Connect;
    GPStateId d_prev_state_id = GPStateId::Connect;
    std::map<GPStateId, std::unique_ptr<GPState>> d_states;
    GPState* d_state = nullptr;
  };

  class GPState
  {
  public:
    virtual ~GPState() = default;
    virtual void process(GoProController&) = 0;
  };

  class StateConnect : public GPState
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
