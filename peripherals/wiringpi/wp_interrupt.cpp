#include "wp_interrupt.h"

#include <wiringPi.h>

using namespace P_WP;

//----------------------------------------------------------------------//
Interrupt::Interrupt(PinNum pin,
		     Mode mode,
		     std::function<void()> callback)
  : d_pin(pin),
    d_mode(mode)
{
  d_pin_state = digitalRead(static_cast<int>(d_pin));

  if (callback.target<void(*)(void)>() == nullptr)
    {
      d_has_error = true;
      return;
    }
  
  if (wiringPiISR(static_cast<int>(pin),
		  static_cast<int>(mode),
		  *callback.target<void(*)(void)>()) < 0)
    {
      d_has_error = true;
    }
}

//----------------------------------------------------------------------//
bool Interrupt::readPin()
{
  d_pin_state = digitalRead(static_cast<int>(d_pin));
  return d_pin_state;
}

//----------------------------------------------------------------------//
bool Interrupt::getCachedPinState()
{
  return d_pin_state;
}
