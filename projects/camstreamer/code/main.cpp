#include "camstreamer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

// TO DO:
//----------------------------------------------------------------------//
// The following will be done for the first release:

// - test pin input control
// - installer script using USB (must exist on the pi image so that only this image can install automatically via usb)
//  - copy compressed file onto usb and plug into pi
//  - check pi version against pi version of install files
//  - install dependencies (curl, wiring pi, boost, omxplayer)
//  - kill current running firmware services
//  - create/clear directory for binary, start script, version doc (all in the same directory)
//  - copy binary
//  - copy start and service script
//  - activate the script
//  - reload and start the scripts
// - wrap LED driver control to show usb installation progress
// - read-only versioning doc

// - test on analog output

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
