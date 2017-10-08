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
    For reliable usage, this class still needs to check the state of the camera before performing a command. Currently, the state is assumed to only change
  */
  
  class FastController final : GoProOwner
  {
  public:
    enum class Req
    {
      Unknown, 
      Connect,
      Photo,
      ToggleRecording,
    };

    enum class StateId
    {
      Disconnected,
      Connected,
      Photo,
      Video
    };

    class Owner
    {
    public:
      Owner() = default;
      virtual ~Owner() = default;
      Owner& operator=(const Owner&) = default;
      Owner(const Owner&) = default;
      //add move constructors
   
    private:
      friend FastController;
      virtual void handleFailedRequest(FastController*, Req) = 0;
      virtual void handleSuccessfulRequest(FastController*, Req) = 0;
    };
    
  public:
    struct CtrlParams
    {
      CtrlParams& setType(CamModel m) { model = m; return *this; }
      CtrlParams& setName(std::string s) { name = std::move(s); return *this; }
      
      CamModel model = CamModel::Hero5;
      std::string name = "Robot";
    };
    
  public:
    FastController(Owner*, const CtrlParams&);

    // If this fails, it continuously tries to reconnect. Should consider setting the maximum number of retry attempts.
    void connectReq(); 
    void takePhotoReq();
    void toggleRecordingReq();

  private:
    class StateInterface;
    class StateConnected;
    class StateDisconnected;
    class StatePhoto;
    class StateVideo;
    
  private:
    //GoProOwner
    void handleCommandSuccessful(GoPro*, Cmd) override;
    void handleCommandFailed(GoPro*, Cmd, GPError) override;

  private:
    void setState(StateId);
    Req getRequest();
    void processCurrentState();
    bool isRecording() { return d_is_recording; }
    void toggleRecState() { d_is_recording = !d_is_recording; }
		
  private:
    friend StateInterface;
    friend StateConnected;
    friend StateDisconnected;
    friend StatePhoto;
    friend StateVideo;
		
    Owner* d_owner = nullptr;

    std::unique_ptr<GoPro> d_gp;
    bool d_is_recording = false;

    StateId d_state_id = StateId::Disconnected;
    std::map<StateId, std::unique_ptr<StateInterface>> d_states;
    StateInterface* d_state = nullptr;

    std::list<Req> d_reqs;

    KERN::CallbackTimer d_timer_recreate_gopro;
  };

  class FastController::StateInterface
  {
  public:
    virtual ~StateInterface() = default;
    // other constructors because of declared descructor
    
    virtual void process(FastController&) = 0;

  protected:
    StateInterface() = default; // forces inheritance
  };

  class FastController::StateConnected : public FastController::StateInterface
  {
    void process(FastController&) override;
  };

  class FastController::StateDisconnected : public FastController::StateInterface
  {
    void process(FastController&) override;
  };
	
  class FastController::StatePhoto : public FastController::StateInterface
  {
    void process(FastController&) override;
  };

  class FastController::StateVideo : public FastController::StateInterface
  {
    void process(FastController&) override;
  };
};
#endif
