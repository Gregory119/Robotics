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
//   - Wifi settings
//   - DONE! custom format (xml in the future with app)
//   - DONE! include pin control type (Connected to digital output pin OR button/switch)
//   - DONE! include pin assignments
// - DONE! LED control classes to indicate operation status and any failures
// - DONE! update usb install script to copy the config/settings file if it exists

// - Use config file wifi settings to set up the wifi
// - python script to restart pi when button connected to pin is pressed. Hold button down to turn off (protect file system). People may not even use this functionality, although it is safer.

// - binary reverse engineering security

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
  CamStreamer cam_streamer("settings.txt");
  cam_streamer.start();
  
  k.run();
  return 0;
}
