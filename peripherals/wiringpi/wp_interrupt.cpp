#include "wp_interrupt.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <wiringPi.h>

static std::vector<std::function<void(void)>> s_callbacks;

using namespace P_WP;

template <unsigned index>
void InterruptFunc(void)
{
  if (s_callbacks.empty())
    {
      assert(false);
      return;
    }
  
  s_callbacks[index]();
}

//----------------------------------------------------------------------//
Interrupt::Interrupt(Owner* o,
		     PinNum pin,
		     Mode mode)
  : d_owner(o),
    d_pin(pin),
    d_mode(mode)
{
  d_pin_state = digitalRead(static_cast<int>(d_pin));
  
  s_callbacks.push_back([this](){
      d_owner.call(&Interrupt::Owner::handleInterrupt,this);
    });
  
  void(*f)(void) = nullptr;
  switch (s_callbacks.size())
    {
    case 1:
      f = InterruptFunc<0>;
      break;

    case 2:
      f = InterruptFunc<1>;
      break;
      
    case 3:
      f = InterruptFunc<2>;
      break;

    case 4:
      f = InterruptFunc<3>;
      break;

    case 5:
      f = InterruptFunc<4>;
      break;

    case 0:
    default:
      assert(false);
      // LOG: current interrupt count limit
      return;
    }

  if (wiringPiISR(static_cast<int>(pin),
		  getWiringPiMode(mode),
		  f) < 0)
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

//----------------------------------------------------------------------//
int Interrupt::getWiringPiMode(Mode mode)
{
  switch (mode)
    {
    case Mode::Rising:
      return INT_EDGE_RISING;
	
    case Mode::Falling:
      return INT_EDGE_FALLING;
      
    case Mode::Both:
      return INT_EDGE_BOTH;
    }
  assert(false);
  // LOG
  d_has_error = true;
}
