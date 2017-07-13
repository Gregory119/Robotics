#ifndef DGP_CONTROLLER_H
#define DGP_CONTROLLER_H

#include "dgp_gopro.h"
//#include "kn_timer.h" //implement this

#include <memory>
#include <vector>

namespace D_GP
{
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
    virtual void handleFailedRequest(GoProController*, Request) = 0;
  };

  class GoProController final : GoProOwner
  {
  public:
    struct GPCtrlParams
    {
      ControlType ctrl_type;
      std::string connect_name;
    };
    
  public:
    GoProController(GoProControllerOwner*, GPCtrlParams);

    void takePicture();
    void takeMultiShot();
    void StartStopRecording();

  private:
    //D_GP::GoPro
    void handleModeSet(GoPro*, Mode) override;
    void handleShutterSet(GoPro*, bool) override;
    void handleFailedCommand(GoPro*, Cmd) override;

  private:
    void connectWithName(const std::string& name);
    void setState(State);
    void processState();
    bool hasStateChanged() { return d_state != d_prev_state; }
    
  private:
    GoProControllerOwner* d_owner = nullptr;
    
    std::unique_ptr<D_GP::GoPro> d_gp;
    D_GP::Request d_req = D_GP::Request::Connect;
    bool d_is_recording = false;
    std::string d_connect_name;
    bool d_multishot_complete = true;

    D_GP::Mode d_state = D_GP::Mode::Disconnected;
    D_GP::Mode d_prev_state = D_GP::Mode::Disconnected;
  };
};
#endif
