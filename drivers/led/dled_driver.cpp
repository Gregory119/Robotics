#include "dled_driver.h"

#include <fstream>

using namespace D_LED;

//----------------------------------------------------------------------//
bool Driver::AdvancedSettings::valid() const
{
  if (flashes_per_sec == 0)
    {
      return false;
    }
  
  if (flash_count == 0)
    {
      return false;
    }
  
  if (start_delay == std::chrono::milliseconds(0))
    {
      return false;
    }
  
  if (cycle == FlashCycle::RepeatLimit &&
      cycle_count == 0)
    {
      return false;
    }
  return true;
}

//----------------------------------------------------------------------//
Driver::Driver(Owner* o, const std::string dir)
  : d_owner(o),
    d_dir(std::move(dir)),
    d_brightness_path(d_dir+"/brightness"),
    d_trigger_path(d_dir+"/trigger"),
    d_delay_on_path(d_dir+"/delay_on"),
    d_delay_off_path(d_dir+"/delay_off")
{
  assert(o != nullptr);
}

//----------------------------------------------------------------------//
void Driver::useMemory()
{
  d_timer.disable();
  internalUseMemory();
}

//----------------------------------------------------------------------//
void Driver::turnOn()
{
  d_timer.disable();
  internalTurnOn();
}

//----------------------------------------------------------------------//
void Driver::turnOff()
{
  d_timer.disable();
  internalTurnOff();
}

//----------------------------------------------------------------------//
void Driver::flashOnOff(const std::chrono::milliseconds& delay_on,
			const std::chrono::milliseconds& delay_off)
{
  d_timer.disable();
  internalFlashOnOff(delay_on, delay_off);
}

//----------------------------------------------------------------------//
void Driver::flashPerSec(unsigned rate)
{
  d_timer.disable();
  internalFlashPerSec(rate);
}

//----------------------------------------------------------------------//
void Driver::flashAdvanced(const AdvancedSettings& set)
{
  d_timer.disable();
  internalFlashAdvanced(set);
}

//----------------------------------------------------------------------//
void Driver::internalUseMemory()
{
  std::fstream file(d_trigger_path);
  if (!file)
    {
      assert(false);
      ownerHandleError(Error::TriggerFile, "D_LED::Driver::internalUseMemory() - Failed to open trigger file.");
      return;
    }
  file << "mmc0" << std::endl;
}

//----------------------------------------------------------------------//
void Driver::internalTurnOn()
{
  std::fstream file(d_brightness_path);
  if (!file)
    {
      assert(false);
      ownerHandleError(Error::BrightnessFile, "D_LED::Driver::internalTurnOn() - Failed to open brightness file.");
      return;
    }
  file << "1" << std::endl;
}

//----------------------------------------------------------------------//
void Driver::internalTurnOff()
{
  std::fstream file(d_brightness_path);
  if (!file)
    {
      assert(false);
      ownerHandleError(Error::BrightnessFile, "D_LED::Driver::internalTurnOff() - Failed to open brightness file.");
      return;
    }
  file << "0" << std::endl;
}

//----------------------------------------------------------------------//
void Driver::internalFlashOnOff(const std::chrono::milliseconds& delay_on,
			const std::chrono::milliseconds& delay_off)
{
  std::fstream trig_file(d_trigger_path);
  if (!trig_file)
    {
      assert(false);
      ownerHandleError(Error::TriggerFile, "D_LED::Driver::internalFlashOnOff() - Failed to open trigger file.");
      return;
    }
  trig_file << "timer" << std::endl;

  std::fstream delay_off_file(d_delay_off_path);
  if (!delay_off_file)
    {
      assert(false);
      ownerHandleError(Error::DelayOffFile, "D_LED::Driver::internalFlashOnOff() - Failed to open delay_off file.");
      return;
    }
  delay_off_file << delay_off.count() << std::endl;

  std::fstream delay_on_file(d_delay_on_path);
  if (!delay_on_file)
    {
      assert(false);
      ownerHandleError(Error::DelayOnFile, "D_LED::Driver::internalFlashOnOff() - Failed to open delay_on file.");
      return;
    }
  delay_on_file << delay_on.count() << std::endl;
}

//----------------------------------------------------------------------//
void Driver::internalFlashPerSec(unsigned rate)
{
  if (rate > 10)
    {
      assert(false);
      ownerHandleError(Error::FlashRateLimit, "D_LED::Driver::internalFlashPerSec() - Flash rate too high.");
      return;
    }

  // total on and off time = 1000/rate;
  // on time = 500/rate
  // off time = 500/rate
  std::chrono::milliseconds half_period(500/rate);
  internalFlashOnOff(half_period, half_period);
}

//----------------------------------------------------------------------//
void Driver::internalFlashAdvanced(const AdvancedSettings& adv)
{
  d_new_flash_cycle = true;
  d_adv_settings = adv;

  if (!adv.valid())
    {
      assert(false);
      d_owner->handleError(this,
			   Error::InvalidAdvSettings,
			   "D_LED::Driver::internalFlashAdvanced() - settings are invalid.");
      return;
    }
  
  // first turn off for begin_delay
  internalTurnOff();
  if (hasError())
    {
      return;
    }
  d_timer.singleShot(std::chrono::milliseconds(d_adv_settings.start_delay));
  d_timer.setTimeoutCallback([this](){
      d_new_flash_cycle = false;
      advCountFlashes();
    });
}

//----------------------------------------------------------------------//
void Driver::ownerHandleError(Error e, const std::string& msg)
{
  d_error = e;
  if (d_owner != nullptr)
    {
      d_owner->handleError(this, e, msg);
    }
}

//----------------------------------------------------------------------//
void Driver::advCountFlashes()
{
  // then flash count times at set rate
  internalFlashPerSec(d_adv_settings.flashes_per_sec);
  if (hasError())
    {
      return;
    }
      
  std::chrono::milliseconds flashing_duration(d_adv_settings.flash_count * 1000 /
					      d_adv_settings.flashes_per_sec);
  d_timer.singleShot(flashing_duration);
  d_timer.setTimeoutCallback([this](){
      advCheckRepeat();
    });
}

//----------------------------------------------------------------------//
void Driver::advCheckRepeat()
{
  switch (d_adv_settings.cycle)
    {
    case FlashCycle::OnceOff:
      d_owner->handleOnceOffFlashCycleEnd(this);
      return;
      
    case FlashCycle::Continuous:
      internalFlashAdvanced(d_adv_settings);
      return;
      
    case FlashCycle::RepeatLimit:
      ++d_flash_cycle_count;
      if (d_flash_cycle_count < d_adv_settings.cycle_count)
	{
	  internalFlashAdvanced(d_adv_settings);
	}
      else
	{
	  d_flash_cycle_count = 0;
	  internalTurnOff();
	}
      return;
    }
}
