#include "camstreamer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

// TO DO:
//----------------------------------------------------------------------//
// The following will be done for the first release:

// - DONE! test basic pin input control
// - DONE! installer script using USB (must exist on the pi image so that only this image can install automatically via usb)
// - DONE! support only edge triggered pin control

// - config file:
//   - custom format (xml in the future with app)
//   - include pin control type (Connected to digital output pin OR button/switch)
//   - include pin assignments
// - update usb install script to check for config file on the usb. LED should indicate any failure.
// - script to restart pi when button connected to pin is pressed. Hold button down to turn off (protect file system)

// - binary reverse engineering security

// - test video analog output

// - cross compile all dependency libraries for all pi versions
// - save omxplayer version
// - test on pi 2 and 3
// - create pi 2 and 3 images

// - release versioning (code base and on pi), branching
//   - private repo purchase: Github, or other.

//----------------------------------------------------------------------//
// To be added after the first release:

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
  // led will use hard drive reads on failure (default)

  KERN::AsioKernel k;
  CamStreamer cam_streamer;
  
  k.run();
  return 0;
}
