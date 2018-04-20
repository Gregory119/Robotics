#include "camstreamer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

// TO DO:
//----------------------------------------------------------------------//
// Still to do for the first release:

// - add logging and copy to usb when inserted so that the user can see the logs (check if logs auto backup) (5)
//   - log the version of the software when it starts
// - encryption (6)

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
// - research common pin control methods using an rc receiver (PWM, HIGH/LOW STATE, SERIAL TYPES). Add support for additional protocols in future releases.

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
