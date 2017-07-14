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
    d_gp_cont->takePhoto(); //uncomment to test, but comment out multishot to see single picture taken
    //d_gp_cont->takeMultiShot();

    //recording
    d_gp_cont->StartStopRecording();
    sleep(5); //recording time
    d_gp_cont->StartStopRecording();
  }
  
private:
  //D_GP::GoProController
  void handleFailedRequest(D_GP::GoProController*) override
  {
    std::cout << "handleFailedRequest: " << std::endl;
  }
  
private:
  std::unique_ptr<D_GP::GoProController> d_gp_cont;
};

int main(int argc, char *argv[])
{
  Test test;

  return EXIT_SUCCESS;
}
