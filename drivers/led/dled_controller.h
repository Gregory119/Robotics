#ifndef D_LED_CONTROLLER_H
#define D_LED_CONTROLLER_H

#include "dled_driver.h"

#include "core_owner.h"
#include <memory>
#include <list>
#include <string>

namespace D_LED
{
  class Controller final : Driver::Owner
  {
  public:
    enum class Req
    {
      Memory,
	On,
	Off,
	FlashOnOff,
	FlashPerSec,
	FlashAdvanced
	};

  public:
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(Controller);
      // This is only called for an advanced flash request that has a limited number of repetitions.
      // It is called once the repetitions are complete.
      virtual void handleFlashCycleEnd(Controller*) {}
    
      virtual void handleReqFailed(Controller*, Req, const std::string& msg) = 0;
      virtual void handleInternalError(Controller*) = 0;
    };
  
  public:
    // Do not make the last character a forward slash. eg. /sys/class/leds/led0
    Controller(Owner*, const std::string& driver_dir = "/sys/class/leds/led0");
    ~Controller();
    // Declaring a move special member function will implicitly delete the copy special members and implicitly not declare the other move special member
    Controller(Controller&&) = delete;

    void setOwner(Owner*);
  
    void useMemory();
    void turnOn();
    void turnOff();
    void flashOnOff(const std::chrono::milliseconds& delay_on,
		    const std::chrono::milliseconds& delay_off);
    void flashPerSec(unsigned); // limited to 10
    void flashAdvanced(const Driver::AdvancedSettings&); // limited to 10

  private:
    // Driver
    void handleFlashCycleEnd(Driver*) override;
    void handleError(Driver*,
		     Driver::Error,
		     const std::string&) override;

  private:
    struct ParamsFlashOnOff
    {
      std::chrono::milliseconds delay_on;
      std::chrono::milliseconds delay_off;
    };
  
  private:
    void startReq(Req);
    void replaceReqFlashAdvanced();
    void popFrontReq();
    void popBackReq();
    void processReq();

    void internalFlashAdvanced();
  
    void ownerReqFailed(const std::string& err_msg);
    void ownerInternalError();
  
  private:
    Owner* d_owner = nullptr;

    std::unique_ptr<Driver> d_led_driver;

    std::vector<Req> d_reqs;
    std::vector<ParamsFlashOnOff> d_reqs_flash_on_off;
    std::vector<unsigned> d_reqs_flash_per_secs;
    std::vector<Driver::AdvancedSettings> d_reqs_adv_settings;
  };
}
#endif
