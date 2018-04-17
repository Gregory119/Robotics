#ifndef D_LED_DRIVER_H
#define D_LED_DRIVER_H

#include "kn_asiocallbacktimer.h"

#include "core_owner.h"
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
      const std::string print() const;
      
      unsigned flashes_per_sec = 0; // must be > 0
      unsigned flash_count = 0; // must be > 0
      std::chrono::milliseconds start_end_between_cycle_delay = std::chrono::milliseconds(500); // at the start, end, and in between every cycle
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
      OWNER_SPECIAL_MEMBERS(Driver);
      virtual void handleError(Driver*, Error, const std::string&) = 0;

      // This is only called for an advanced flash request that has a limited number of repetitions.
      // It is called once the repetitions are complete.
      virtual void handleFlashCycleEnd(Driver*) = 0;
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

    void internalUseMemory();
    void internalTurnOn();

    // If set, the function callback parameter will be called after the delay.
    void turnOnDuration(const std::chrono::milliseconds& delay,
			std::function<void()> f = nullptr);
    void turnOffDuration(const std::chrono::milliseconds& delay,
			 std::function<void()> f = nullptr);
    
    void internalTurnOff();

    // If count is negative, then the flashing will never stop, otherwise specify the number of flashes before stopping.
    // If set, the function callback parameter will be called after count flashes.
    void internalFlashOnOffCount(const std::chrono::milliseconds& delay_on,
				 const std::chrono::milliseconds& delay_off,
				 int count = -1,
				 std::function<void()> f=nullptr);

    // If count is negative, then the flashing will never stop, otherwise specify the number of flashes before stopping.
    // If set, the function callback parameter will be called after count flashes
    void internalFlashPerSecCount(unsigned rate,
				  int count=-1,
				  std::function<void()> f=nullptr); // rate is limited to 10
    
    void internalFlashAdvanced(const AdvancedSettings&); // limited to 10

    void disableTimers();

  private:
    Owner* d_owner = nullptr;
    
    const std::string d_dir;
    const std::string d_brightness_path;
    const std::string d_trigger_path;
    const std::string d_delay_on_path;
    const std::string d_delay_off_path;

    Error d_error = Error::None;

    KERN::AsioCallbackTimer d_on_off_timer = KERN::AsioCallbackTimer("D_LED::Driver On/Off Timer");
    bool d_is_led_on = false;
    unsigned d_flash_count = 0;
    
    AdvancedSettings d_adv_settings;
    KERN::AsioCallbackTimer d_adv_timer = KERN::AsioCallbackTimer("D_LED::Driver Advanced Settings Cycle Timer");
    unsigned d_flash_cycle_count = 0;
  };
};

#endif
