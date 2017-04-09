#ifndef DGP_CONTROLLER_H
#define DGP_CONTROLLER_H

#include "dgp_gopro.h"
#include <memory>
#include <deque>

namespace D_GP
{
  class GoProController;
  class GoProControllerOwner
  {
  public:
    GoProControllerOwner() = default;
    virtual ~GoProControllerOwner();
    GoProControllerOwner& operator=(const GoProControllerOwner&) = default;
    GoProControllerOwner(const GoProControllerOwner&) = default;
   
  private:
    friend GoProController;
    virtual void handleFailedRequest(GoProController*, Request) = 0;
  };

  class GoProController final : GoProOwner
  {
  public:
    GoProController(GoProControllerOwner*, ControlType);

    void connectWithName(const std::string& name);
    void takePicture();
    void takeMultiShot();
    void StartStopRecording();

  private:
    //D_GP::GoPro
    void handleModeChanged(GoPro*, Mode) override;
    void handleShutterSet(GoPro*, bool) override;
    void handleFailedCommand(GoPro*, Cmd) override;

    void toggleRecording() { d_is_recording = !d_is_recording; }

  private:
    GoProControllerOwner* d_owner;
    
    std::unique_ptr<D_GP::GoPro> d_gp = nullptr;
    std::deque<Request> d_gp_cmd_list;
    bool d_is_recording = false;
  };
};
#endif
