#include "dgp_controller.h"

#include <iostream>
//#include <cstdlib> //exit success/failure
#include <memory> //std pointers

class Test : public D_GP::GoProControllerOwner
{
public:  
  Test()
  {
    d_gp_cont.reset(new D_GP::GoProController(this, D_GP::ControlType::Simple));
    d_gp_cont->connectWithName("BushBot");
    d_gp_cont->takePicture();
  }
  
private:
  //D_GP::GoProController
  void handleFailedRequest(D_GP::GoProController*, D_GP::Request) override
  {
    std::cout << "handleFailedCommand" << std::endl;
  }
  
private:
  std::unique_ptr<D_GP::GoProController> d_gp_cont;
};

int main(int argc, char *argv[])
{
  Test test;

  return EXIT_SUCCESS;
}
