#ifndef D_LED_DRIVER_H
#define D_LED_DRIVER_H

#include <chrono>
#include <functional>
#include <string>

namespace D_LED
{
  class Driver final
  {
  public:
    // Do not make the last character a forward slash. eg. /sys/class/leds/led0
    explicit Driver(const std::string dir = "/sys/class/leds/led0"); 

    bool useMemory();
    bool turnOn();
    bool turnOff();
    bool flashOnOff(const std::chrono::milliseconds& delay_on,
		    const std::chrono::milliseconds& delay_off);

  private:
    const std::string d_dir;
    const std::string d_brightness_path;
    const std::string d_trigger_path;
    const std::string d_delay_on_path;
    const std::string d_delay_off_path;
  };
};

#endif
