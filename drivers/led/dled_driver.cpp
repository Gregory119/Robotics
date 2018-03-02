#include "dled_driver.h"

#include <fstream>
#include <iostream>

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
  d_adv_timer.disableIfEnabled();
  internalUseMemory();
}

//----------------------------------------------------------------------//
void Driver::turnOn()
{
  d_adv_timer.disableIfEnabled();
  internalTurnOn();
}

//----------------------------------------------------------------------//
void Driver::turnOff()
{
  d_adv_timer.disableIfEnabled();
  internalTurnOff();
}

//----------------------------------------------------------------------//
void Driver::flashOnOff(const std::chrono::milliseconds& delay_on,
			const std::chrono::milliseconds& delay_off)
{
  d_adv_timer.disableIfEnabled();
  internalFlashOnOffCount(delay_on, delay_off);
}

//----------------------------------------------------------------------//
void Driver::flashPerSec(unsigned rate)
{
  d_adv_timer.disableIfEnabled();
  internalFlashPerSecCount(rate);
}

//----------------------------------------------------------------------//
void Driver::flashAdvanced(const AdvancedSettings& set)
{
  d_adv_timer.disableIfEnabled();
  internalFlashAdvanced(set);
}

//----------------------------------------------------------------------//
void Driver::internalUseMemory()
{
  std::fstream file(d_trigger_path);
  if (!file)
    {
      assert(false);
      ownerHandleError(Error::TriggerFile, "internalUseMemory - Failed to open trigger file.");
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
      ownerHandleError(Error::BrightnessFile, "internalTurnOn - Failed to open brightness file.");
      return;
    }
  file << "1" << std::endl;
}

//----------------------------------------------------------------------//
void Driver::turnOnDuration(const std::chrono::milliseconds& delay,
			    std::function<void()> f)
{
  internalTurnOn();
  if (hasError())
    {
      assert(false);
      return;
    }
  d_on_off_timer.disableIfEnabled();
  if (f)
    {
      d_on_off_timer.setTimeoutCallback(f);
      d_on_off_timer.singleShot(delay);
    }
}

//----------------------------------------------------------------------//
void Driver::internalTurnOff()
{
  std::fstream file(d_brightness_path);
  if (!file)
    {
      assert(false);
      ownerHandleError(Error::BrightnessFile, "internalTurnOff - Failed to open brightness file.");
      return;
    }
  file << "0" << std::endl;
}

//----------------------------------------------------------------------//
void Driver::turnOffDuration(const std::chrono::milliseconds& delay,
			     std::function<void()> f)
{  
  internalTurnOff();
  if (hasError())
    {
      assert(false);
      return;
    }
  
  d_on_off_timer.disableIfEnabled();
  if (f)
    {
      d_on_off_timer.setTimeoutCallback(f);
      d_on_off_timer.singleShot(delay);
    }
}

//----------------------------------------------------------------------//
void Driver::internalFlashOnOffCount(const std::chrono::milliseconds& delay_on,
				     const std::chrono::milliseconds& delay_off,
				     int count,
				     std::function<void()> f)
{
  if (count > 0)
    {
      turnOnDuration(delay_on, [=](){
	  // the following line is needed to ensure that a copy (not a reference) of delay_off is made
	  std::chrono::milliseconds delay_off_copy = delay_off;
	  turnOffDuration(delay_off_copy,[=](){
	      // decrease the count after an on and off delay until it gets to zero
	      int new_count = count-1;
	      if (new_count == 0)
		{
		  f();
		}
	      else
		{
		  internalFlashOnOffCount(delay_on,delay_off,new_count,f);
		}
	    });
	});
    }
  else if (count < 0)
    {
      turnOnDuration(delay_on, [=](){
	  turnOffDuration(delay_off,[=](){
	      internalFlashOnOffCount(delay_on,delay_off,count);
	    });
	});
    }
}

//----------------------------------------------------------------------//
void Driver::internalFlashPerSecCount(unsigned rate,
				      int count,
				      std::function<void()> f)
{
  if (rate > 10)
    {
      assert(false);
      ownerHandleError(Error::FlashRateLimit, "internalFlashPerSec - Flash rate too high.");
      return;
    }

  // total on and off time = 1000/rate;
  // on time = 500/rate
  // off time = 500/rate
  std::chrono::milliseconds half_period(500/rate);
  internalFlashOnOffCount(half_period, half_period, count, f);
}

//----------------------------------------------------------------------//
void Driver::internalFlashAdvanced(const AdvancedSettings& adv)
{
  if (!adv.valid())
    {
      assert(false);
      d_owner->handleError(this,
			   Error::InvalidAdvSettings,
			   "D_LED::Driver::internalFlashAdvanced() - settings are invalid.");
      return;
    }
  d_adv_settings = adv;
  
  // only do begin delay if greater than zero
  if (d_adv_settings.start_end_between_cycle_delay == std::chrono::milliseconds(0))
    {
      advCountFlashes();
      return;
    }

  // first turn off for begin_delay
  internalTurnOff();
  if (hasError())
    {
      return;
    }
  
  d_adv_timer.setTimeoutCallback([this](){
      advCountFlashes();
    });
  d_adv_timer.singleShot(d_adv_settings.start_end_between_cycle_delay);
}

//----------------------------------------------------------------------//
void Driver::ownerHandleError(Error e, const std::string& msg)
{
  d_error = e;
  std::string final_msg = "D_LED::Driver::";
  final_msg += msg;
  if (d_owner != nullptr)
    {
      d_owner->handleError(this, e, final_msg);
    }
}

//----------------------------------------------------------------------//
void Driver::advCountFlashes()
{
  // then flash count times at set rate
  internalFlashPerSecCount(d_adv_settings.flashes_per_sec,
			   d_adv_settings.flash_count,
			   [this](){
			     advCheckRepeat();
			   });
}

//----------------------------------------------------------------------//
void Driver::advCheckRepeat()
{
  switch (d_adv_settings.cycle)
    {
    case FlashCycle::OnceOff:
      d_adv_timer.setTimeoutCallback([this](){
	  d_owner->handleOnceOffFlashCycleEnd(this);
	});
      d_adv_timer.singleShot(d_adv_settings.start_end_between_cycle_delay);
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
