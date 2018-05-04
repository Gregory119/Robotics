#include "camstreamer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

// TO DO:
//----------------------------------------------------------------------//
// FREE VERSION:

// script to decrypt firmware after copying from usb

// image and firmware version tracking
//  - move to new private repo (GitLab?)
//  - move libraries to a separate repo from the projects.
// log firmware version
// log image version and architecture
// log detected architecture
// log file system version and details

// compile in RELEASE for testing
// setup image for pi 2
// final pi 2 test
// setup image and cross compile for pi 3
// final pi 3 test
// setup image for pi 1
// final test for pi 1

// - test and support most GoPro models (consider adding older ones if time permits - add to config file) 

// PAID VERSION:
// - copy the license request file to the usb
// - encryption (6)

//----------------------------------------------------------------------//
// To be added after the first release:

// - add configuration of which modes to include in the next mode command

// - support older GoPro models
// - add other rc protocols

// - config xml doc (will support app config)

// create app (app and device side additions)
// - configure GoPro wifi connection details
// - set either analog or hdmi output
// - set mode and trigger buttons (for testing)
// - install from usb button
// - set pins for controls
// - have a history/live log section for showing any operation/installation problems OR a single state name with description.

int main()
{
  wiringPiSetup(); // terminates on failure

  KERN::AsioKernel k;
  CamStreamer cam_streamer("GoPie_userconfig.txt");

  cam_streamer.start();
  
  k.run();
  return 0;
}
