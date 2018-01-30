#ifndef D_LED_DRIVER_H
#define D_LED_DRIVER_H

#include "kn_asiocallbacktimer.h"

#include <chrono>
#include <functional>
#include <string>

namespace D_LED
{
  class Driver
  {
  public:
    enum class FlashCycle
    {
      OnceOff,
      Continuous,
      RepeatLimit
    };
    
    struct AdvancedSettings
    {
      bool valid() const;
      
      unsigned flashes_per_sec = 0; // must be > 0
      unsigned flash_count = 0; // must be > 0
      std::chrono::milliseconds start_delay = std::chrono::milliseconds(500);
      FlashCycle cycle = FlashCycle::Continuous;
      unsigned cycle_count = 0; // must be > 0 if on a limited repeat flash cycle
    };

    enum class Error
    {
      TriggerFile,
	BrightnessFile,
	DelayOnFile,
	DelayOffFile,
	FlashRateLimit,
	InvalidAdvSettings,
	None
    };
    
  public:
    class Owner
    {
    protected:
      Owner() = default;
      // declared move special members implicitly delete copy special members
      Owner(Owner&&) = delete;
      Owner& operator=(Owner&&) = delete;
      // move special members are implicitly not declared with a declared destructor
      ~Owner() = default;
	
    private:
      friend class Driver;
      virtual void handleError(Driver*, Error, const std::string&) = 0;
    };
    
  public:
    // Do not make the last character a forward slash. eg. /sys/class/leds/led0
    Driver(Owner*, const std::string dir = "/sys/class/leds/led0");
    virtual ~Driver() = default; // Can be inherited, used standalone, and polymorphically. Move members are now not declared.
    Driver(const Driver&) = default;
    Driver& operator=(const Driver&) = default;
    Driver(Driver&&) = default;
    Driver& operator=(Driver&&) = default;

    void useMemory();
    void turnOn();
    void turnOff();
    void flashOnOff(const std::chrono::milliseconds& delay_on,
		    const std::chrono::milliseconds& delay_off);
    void flashPerSec(unsigned); // limited to 10
    void flashAdvanced(const AdvancedSettings&); // limited to 10

    bool hasError() { return d_error != Error::None; }

  private:
    void ownerHandleError(Error, const std::string& msg);
    void advCountFlashes();
    void advCheckRepeat();
    
  private:
    Owner* d_owner = nullptr;
    
    const std::string d_dir;
    const std::string d_brightness_path;
    const std::string d_trigger_path;
    const std::string d_delay_on_path;
    const std::string d_delay_off_path;

    Error d_error = Error::None;
    
    AdvancedSettings d_adv_settings;
    KERN::AsioCallbackTimer d_timer;
    unsigned d_flash_cycle_count = 0;
    bool d_new_flash_cycle = false;
  };
};

#endif
