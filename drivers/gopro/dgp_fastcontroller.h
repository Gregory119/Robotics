#ifndef DGP_FASTCONTROLLER_H
#define DGP_FASTCONTROLLER_H

#include "dgp_gopro.h"
#include "kn_asiocallbacktimer.h"

#include <list>
#include <memory>
#include <map>

namespace D_GP
{
  /*
    TODO: 
    - For reliable usage, this class still needs to check the state of the camera before performing a command. Currently, the state is assumed to only change
    - There needs to be a pairing process in which the connect command is sent. Sending it with every command from a disconnected state has caused a weird problem, whereby setting the mode does not set it to the requested mode.
  */
  
  class FastController final : GoPro::Owner
  {
  public:
    enum class Req
    {
      Unknown, 
      Connect,
      Photo,
      ToggleRecording
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
      Owner& operator=(const Owner&) = default;
      Owner(const Owner&) = default;
      Owner(Owner&&) = default;
      Owner& operator=(Owner&&) = default;

    protected:
      Owner() = default;
      virtual ~Owner() = default;
      
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
    //GoPro::Owner
    void handleCommandSuccessful(GoPro*, GoPro::Cmd) override;
    void handleCommandFailed(GoPro*, GoPro::Cmd, GoPro::Error) override;
    void handleStreamDown(GoPro*) override { /* nothing for now*/ }

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
    
    KERN::AsioCallbackTimer d_timer_recreate_gopro;
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
