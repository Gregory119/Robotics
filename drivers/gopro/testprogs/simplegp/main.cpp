#include "dgp_controller.h"

#include <iostream>
//#include <cstdlib> //exit success/failure
#include <memory> //std pointers

#include <unistd.h>

class Test : public D_GP::GoProControllerOwner
{
public:  
  Test()
    : d_gp_cont(new D_GP::GoProController(this, D_GP::GoProController::GPCtrlParams()))
  {    
    d_gp_cont->takePhotoReq(); //uncomment to test, but comment out multishot to see single picture taken
    //d_gp_cont->takeMultiShotReq();

    //recording
    d_gp_cont->toggleRecordingReq();
    sleep(5); //recording time
    d_gp_cont->toggleRecordingReq();
  }
  
private:
  //D_GP::GoProController
  void handleFailedRequest(D_GP::GoProController*,
			   D_GP::GoProControllerReq req) override
  {
    std::cout << "handleFailedRequest: " << std::endl;
  }

  void handleSuccessfulRequest(D_GP::GoProController*, D_GP::GoProControllerReq) override
  {
    std::cout << "handleSuccessfulRequest: " << std::endl;
  }
  
private:
  std::unique_ptr<D_GP::GoProController> d_gp_cont;
};

int main(int argc, char *argv[])
{
  Test test;

  return EXIT_SUCCESS;
}
