#ifndef LEDCONTROLLER_H
#define LEDCONTROLLER_H

#include "dled_driver.h"

#include <string>
#include <memory>

class LedController final : D_LED::Driver::Owner
{
 public:
  enum class State
  {
    InvalidConfig,
      Connecting,
      Connected,
      CommandSuccessful,
      InternalFailure,
      Unknown
  };

 public:
  class Owner
  {
  protected:
    Owner() = default;
    // Declaring a move special member function will implicitly delete the copy special members and implicitly not declare the other move special member
    Owner(Owner&&) = delete;
    // Move special member functions are not declared when the desctructor is.
    ~Owner() = default; // must inherit and non-polymorphic
    
  private:
    friend class LedController;
    virtual void handleStateChange(LedController*, State) = 0;
    virtual void handleOnceOffFlashCycleEnd(LedController*, State) = 0;
    virtual void handleError(LedController*, const std::string& msg) = 0;
  };
  
 public:
  // Do not make the last character a forward slash. eg. /sys/class/leds/led0
  LedController(Owner*, const std::string& driver_dir = "/sys/class/leds/led0");
  // Declaring a move special member function will implicitly delete the copy special members and implicitly not declare the other move special member
  LedController(LedController&&) = delete;

  void setState(State);

  void useMemory();
  void turnOn();
  void turnOff();
  void flashOnOff(const std::chrono::milliseconds& delay_on,
		  const std::chrono::milliseconds& delay_off);
  void flashPerSec(unsigned); // limited to 10
  void flashAdvanced(const D_LED::Driver::AdvancedSettings&); // limited to 10

 private:
  // D_LED::Driver
  void handleOnceOffFlashCycleEnd(D_LED::Driver*) override;
  void handleError(D_LED::Driver*,
		   D_LED::Driver::Error,
		   const std::string&) override;
  
 private:
  Owner* d_owner = nullptr;
  State d_state = State::Unknown;

  std::unique_ptr<D_LED::Driver> d_led_driver;
};

#endif
