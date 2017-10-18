#include "dgp_modecontroller.h"

#include "kn_asiokernel.h"

#include <iostream>
//#include <cstdlib> //exit success/failure
#include <memory> //std pointers

#include <unistd.h>

class Test : public D_GP::ModeController::Owner
{
public:  
  Test()
    : d_gp_cont(new D_GP::ModeController(this, D_GP::ModeController::CtrlParams()))
  {    
  }

  void connect()
  {
    d_gp_cont->connect();
  }

  void nextMode()
  {
    d_gp_cont->nextMode();
  }

  void trigger()
  {
    d_gp_cont->trigger();
  }

  void startStream()
  {
    d_gp_cont->startStream();
  }
  
private:
  //D_GP::ModeController
  void handleFailedRequest(D_GP::ModeController*,
			   D_GP::ModeController::Req req) override
  {
    std::cout << "handleFailedRequest: " << std::endl;
  }

  void handleSuccessfulRequest(D_GP::ModeController*, D_GP::ModeController::Req) override
  {
    std::cout << "handleSuccessfulRequest: " << std::endl;
  }
  
private:
  std::unique_ptr<D_GP::ModeController> d_gp_cont;
};

//----------------------------------------------------------------------//
void PrintHelp()
{
  std::cout << "./gp_modecontroller [command]\n" 
	    << "[command]\n" 
	    << "connect - request to pair\n" 
	    << "next_mode - request next mode\n"
	    << "trigger - request to trigger\n"
	    << "stream - request to start stream"
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

  KERN::AsioKernel k;
  Test test;

  if (std::string(argv[argc-1]) == "connect")
    {
      test.connect();
      k.run();
    }
  else if (std::string(argv[argc-1]) == "next_mode")
    {
      test.nextMode();
      k.run();
    }
  else if (std::string(argv[argc-1]) == "trigger")
    {
      test.trigger();
      k.run();
    }
  else if (std::string(argv[argc-1]) == "stream")
    {
      test.startStream();
      k.run();
    }
  else
    {
      PrintHelp();
    }

  return EXIT_SUCCESS;
}
