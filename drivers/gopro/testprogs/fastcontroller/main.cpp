#include "dgp_fastcontroller.h"

#include "kn_stdkernel.h"

#include <iostream>
//#include <cstdlib> //exit success/failure
#include <memory> //std pointers

#include <unistd.h>

class Test : public D_GP::FastController::Owner
{
public:  
  Test()
    : d_gp_cont(new D_GP::FastController(this, D_GP::FastController::CtrlParams()))
  {    
  }

  void photo()
  {
    d_gp_cont->takePhotoReq();
  }

  void recording()
  {
    d_gp_cont->toggleRecordingReq();
  }
  
private:
  //D_GP::FastController
  void handleFailedRequest(D_GP::FastController*,
			   D_GP::FastController::Req req) override
  {
    std::cout << "handleFailedRequest: " << std::endl;
  }

  void handleSuccessfulRequest(D_GP::FastController*, D_GP::FastController::Req) override
  {
    std::cout << "handleSuccessfulRequest: " << std::endl;
  }
  
private:
  std::unique_ptr<D_GP::FastController> d_gp_cont;
};

//----------------------------------------------------------------------//
void PrintHelp()
{
  std::cout << "./gp_fastcontroller [command]\n" 
	    << "[command]\n" 
	    << "photo - take a photo\n" 
	    << "recording - start/stop the recording"
	    << std::endl;
}

//----------------------------------------------------------------------//
int main(int argc, char *argv[])
{
  if ((argc == 1) || (argc != 2))
    {
      PrintHelp();
      return EXIT_SUCCESS;
    }

  KERN::StdKernel k;
  Test test;

  // take photo
  if (std::string(argv[argc-1]) == "photo")
    {
      test.photo();
      k.run();
    }
  // toggle recording
  else if (std::string(argv[argc-1]) == "recording")
    {
      test.recording();
      k.run();
    }
  else
    {
      PrintHelp();
    }

  return EXIT_SUCCESS;
}
