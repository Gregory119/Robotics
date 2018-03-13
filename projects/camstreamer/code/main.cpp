#include "camstreamer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

//ORDER:
// RC transmitter and receiver
// pi 2, 3, sd cards, double check power requirements
// 4/8 GB sd cards for a small image size => will need to expand to used sd card size


// TO DO:
//----------------------------------------------------------------------//
// The following will be done for the first release:

// - test/modify usb scripts
// - pin shutdown => led should use memory
// - change pin detection to use with RC receivers (PPM) also check if they do high and low states
// - add logging and copy to usb when inserted so that the user can see the logs (check if logs auto backup)
// - encryption

// - test video analog output
// - test and support most GoPro models (consider adding older ones if time permits - add to config file)

// - cross compile all dependency libraries for all pi versions
// - save omxplayer version
// - test on pi 2 and 3
// - create pi 2 and 3 images

// - release versioning (code base and on pi), branching
//   - private repo purchase: Github, or other.

//----------------------------------------------------------------------//
// To be added after the first release:

// - support older GoPro models
// - research common pin control methods using an rc receiver (PWM, HIGH/LOW STATE, SERIAL TYPES). These will be supported in future releases.

// - logs (helpful to some users)

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
  CamStreamer cam_streamer("config.txt");
  cam_streamer.start();
  
  k.run();
  return 0;
}
