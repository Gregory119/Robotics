#include <iostream>
#include <chrono>
#include <iostream>
//#include <thread>

#include "kn_asiokernel.h"

#include "kn_asiocallbacktimer.h"

int main(int argc, char** argv)
{
  KERN::AsioKernel k;

  KERN::AsioCallbackTimer timer1("test timer1");
  KERN::AsioCallbackTimer timer2("test timer2");
  timer1.setTimeoutCallback([](){
      std::cout << "timer 1" << std::endl;
    });
  timer2.setTimeoutCallback([](){
      std::cout << "timer 2" << std::endl;
    });
  timer1.restart(std::chrono::seconds(10));
  timer1.restart(std::chrono::seconds(1));
  timer2.restart(std::chrono::milliseconds(50));
  
  k.run();
  return 0;
}
