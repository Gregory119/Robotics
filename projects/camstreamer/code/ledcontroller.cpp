#include "ledcontroller.h"

//----------------------------------------------------------------------//
LedController::LedController(Owner* o, const std::string& driver_dir)
  : d_owner(o),
    d_led_driver(new D_LED::Driver(this, driver_dir))
{
  assert(o != nullptr);
}

//----------------------------------------------------------------------//
void LedController::setState(State s)
{
  if (d_state == s)
    {
      return;
    }

  d_state = s;
  d_owner->handleStateChange(this, s);
}

//----------------------------------------------------------------------//
void LedController::useMemory()
{
  d_led_driver->useMemory();
}

//----------------------------------------------------------------------//
void LedController::turnOn()
{
  d_led_driver->turnOn();
}

//----------------------------------------------------------------------//
void LedController::turnOff()
{
  d_led_driver->turnOff();
}

//----------------------------------------------------------------------//
void LedController::flashOnOff(const std::chrono::milliseconds& delay_on,
			       const std::chrono::milliseconds& delay_off)
{
  d_led_driver->flashOnOff(delay_on, delay_off);
}

//----------------------------------------------------------------------//
void LedController::flashPerSec(unsigned rate)
{
  d_led_driver->flashPerSec(rate);
}

//----------------------------------------------------------------------//
void LedController::flashAdvanced(const D_LED::Driver::AdvancedSettings& set)
{
  d_led_driver->flashAdvanced(set);
}

//----------------------------------------------------------------------//
void LedController::handleOnceOffFlashCycleEnd(D_LED::Driver*)
{
  d_owner->handleOnceOffFlashCycleEnd(this, d_state);
}

//----------------------------------------------------------------------//
void LedController::handleError(D_LED::Driver* driver,
				D_LED::Driver::Error e,
				const std::string& msg)
{
  d_owner->handleError(this, "LedController::handleError() " + msg);
}
