#include "camstreamer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

// TO DO:
// - cam streamer class (map pin inputs to gopro control) - TEST!!!

// - optimize gopro streaming settings
//   - increase bitrate to reduce dropped packets
//   - sniff gopro app command for keeping stream up (the restart one causes the stream to stop temporarily)

// create app (app and device side additions)
// - configure GoPro wifi connection details
// - set either analog or hdmi output
// - set mode and trigger buttons (for testing)
// - install from usb button
// - set pins for controls
// - have a history/live log section for showing any operation/installation problems OR a single state name with description.

// - installer script using USB

// - wrap LED driver control and use in camstreamer

int main()
{
  wiringPiSetup(); // terminates on failure
  // led will use hard drive reads on failure (default)

  KERN::AsioKernel k;
  CamStreamer cam_streamer;
  
  k.run();
  return 0;
}
