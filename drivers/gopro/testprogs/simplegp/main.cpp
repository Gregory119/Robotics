#include "dgp_controller.h"

#include <iostream>
//#include <cstdlib> //exit success/failure
#include <memory> //std pointers

#include <unistd.h>

class Test : public D_GP::GoProControllerOwner
{
public:  
  Test()
  {
    d_gp_cont.reset(new D_GP::GoProController(this, D_GP::ControlType::Simple));
    d_gp_cont->connectWithName("BushBot");
    //d_gp_cont->takePicture(); //uncomment to test, but comment out multishot to see single picture taken
    //d_gp_cont->takeMultiShot();

    //recording
    d_gp_cont->StartStopRecording();
    sleep(5); //recording time
    d_gp_cont->StartStopRecording();
  }
  
private:
  //D_GP::GoProController
  void handleFailedRequest(D_GP::GoProController*, D_GP::Request req) override
  {
    std::cout << "handleFailedRequest: " << D_GP::reqToString(req) << std::endl;
  }
  
private:
  std::unique_ptr<D_GP::GoProController> d_gp_cont;
};

int main(int argc, char *argv[])
{
  Test test;

  return EXIT_SUCCESS;
}
