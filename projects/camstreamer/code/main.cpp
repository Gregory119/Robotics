#include "camstreamer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

// TO DO:
//----------------------------------------------------------------------//
// The following will be done for the first release:

// - test basic pin input control
// - research common pin control methods using an rc receiver (PWM, HIGH/LOW STATE, SERIAL TYPES)
// - DONE! installer script using USB (must exist on the pi image so that only this image can install automatically via usb)

// - test on analog output

// release versioning (code base and on pi), branching

// - cross compile all dependency libraries for all pi versions
// - save omxplayer version
// - create pi 2 and 3 images
// - test on pi 2 and 3

// - binary reverse engineering security

//----------------------------------------------------------------------//
// To be added after the first release:

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
  // led will use hard drive reads on failure (default)

  KERN::AsioKernel k;
  CamStreamer cam_streamer;
  
  k.run();
  return 0;
}
