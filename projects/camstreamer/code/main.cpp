#include "pipins.h"
#include "camstreamer.h"

#include "kn_asiokernel.h"

#include <iostream>

int main()
{
  KERN::AsioKernel k;

  // cam streamer pins
  Robot::Params params(SERVONUM::steering,
		       SERVONUM::motor);
  // TO DO:
  // - start script to use omxplayer
  // - wrap pin control wiringPi
  // - wrap LED driver control
  // - cam streamer class
  // - installer script using USB
  // - optimize gopro streaming settings
  
  // create cam streamer
  // initializes pin modes
  // start pinging a request to check for active wifi (this can be the connect command to assist with pairing) - signal this state with the green LED
  // pinging stops once connected (signal with LED), starts streaming, and reads pins in short intervals to check a change in state
  // pin state changes (signal with LED) trigger camera requests
  // if a request fails, then reset control and start the pinging again
  Robot r(params);
  
  k.run();
  return 0;
}
