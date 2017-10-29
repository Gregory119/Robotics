#include "camstreamer.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <wiringPi.h>
#include <iostream>

// TO DO:
// - cam streamer class (map pin inputs to gopro control)
// - start script to use omxplayer
// - test

// - wrap LED driver control and use in camstreamer
// - installer script using USB
// - optimize gopro streaming settings
//   - increase bitrate to reduce dropped packets
//   - sniff gopro app command for keeping stream up (the restart one causes the stream to stop temporarily)

int main()
{
  wiringPiSetup(); // terminates on failure
  // led will use hard drive reads on failure (default)

  KERN::AsioKernel k;
  CamStreamer cam_streamer;
  
  k.run();
  return 0;
}
