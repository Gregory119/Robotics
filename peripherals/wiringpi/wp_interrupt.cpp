#include "wp_interrupt.h"

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <wiringPi.h>

using namespace P_WP;

static std::unordered_map<int,std::function<void(void)>> s_callbacks; // pin num to callback func

template <PinNum index>
void InterruptFunc(void)
{
  s_callbacks[static_cast<int>(index)]();
}

//----------------------------------------------------------------------//
Interrupt::Interrupt(Owner* o,
		     PinNum pin,
		     EdgeMode mode)
  : d_owner(o),
    d_pin(pin),
    d_mode(mode),
    d_callback_index(static_cast<int>(pin))
{
  d_error_timer.setTimeoutCallback([this](){
      d_owner.call(&Owner::handleError,
		   this,
		   d_error,
		   d_err_msg);
    });

  if (s_callbacks.size() >= 2)
    {
      assert(false);
      d_error = Error::MaxInstances;
      d_err_msg = "Interrupt::Interrupt - The limit on the number of interrupt instances is 2.";
      d_error_timer.singleShotZero();
      return;
    }

  try
    {
      s_callbacks.at(d_callback_index);
      // success at this point => pin used
      d_error = Error::DuplicatePin;
      std::ostringstream stream("Interrupt::Interrupt - Failed to create an interrupt for pin '",
				std::ios_base::app);
      stream << static_cast<int>(d_pin) << "' because one already exists.";
      d_err_msg = stream.str();
      d_error_timer.singleShotZero();
    }
  catch (...)
    {
      // pin is available so continue
    }

  // callback function for interrupt
  switch (d_pin)
    {
    case PinNum::W8:
      d_temp_callback = InterruptFunc<PinNum::W8>;
      break;
      
    case PinNum::W9:
      d_temp_callback = InterruptFunc<PinNum::W9>;
      break;
      
    case PinNum::W7:
      d_temp_callback = InterruptFunc<PinNum::W7>;
      break;
      
    case PinNum::W15:
      d_temp_callback = InterruptFunc<PinNum::W15>;
      break;
      
    case PinNum::W16:
      d_temp_callback = InterruptFunc<PinNum::W16>;
      break;
      
    case PinNum::W0:
      d_temp_callback = InterruptFunc<PinNum::W0>;
      break;
      
    case PinNum::W1:
      d_temp_callback = InterruptFunc<PinNum::W1>;
      break;
      
    case PinNum::W2:
      d_temp_callback = InterruptFunc<PinNum::W2>;
      break;
      
    case PinNum::W3:
      d_temp_callback = InterruptFunc<PinNum::W3>;
      break;
      
    case PinNum::W4:
      d_temp_callback = InterruptFunc<PinNum::W4>;
      break;
      
    case PinNum::W5:
      d_temp_callback = InterruptFunc<PinNum::W5>;
      break;
      
    case PinNum::W12:
      d_temp_callback = InterruptFunc<PinNum::W12>;
      break;
      
    case PinNum::W13:
      d_temp_callback = InterruptFunc<PinNum::W13>;
      break;
      
    case PinNum::W6:
      d_temp_callback = InterruptFunc<PinNum::W6>;
      break;
      
    case PinNum::W14:
      d_temp_callback = InterruptFunc<PinNum::W14>;
      break;
      
    case PinNum::W10:
      d_temp_callback = InterruptFunc<PinNum::W10>;
      break;
      
    case PinNum::W11:
      d_temp_callback = InterruptFunc<PinNum::W11>;
      break;
      
    case PinNum::W30:
      d_temp_callback = InterruptFunc<PinNum::W30>;
      break;
      
    case PinNum::W31:
      d_temp_callback = InterruptFunc<PinNum::W31>;
      break;
      
    case PinNum::W21:
      d_temp_callback = InterruptFunc<PinNum::W21>;
      break;
      
    case PinNum::W22:
      d_temp_callback = InterruptFunc<PinNum::W22>;
      break;
      
    case PinNum::W26:
      d_temp_callback = InterruptFunc<PinNum::W26>;
      break;
      
    case PinNum::W23:
      d_temp_callback = InterruptFunc<PinNum::W23>;
      break;
      
    case PinNum::W24:
      d_temp_callback = InterruptFunc<PinNum::W24>;
      break;
      
    case PinNum::W27:
      d_temp_callback = InterruptFunc<PinNum::W27>;
      break;
      
    case PinNum::W25:
      d_temp_callback = InterruptFunc<PinNum::W25>;
      break;
      
    case PinNum::W28:
      d_temp_callback = InterruptFunc<PinNum::W28>;
      break;
      
    case PinNum::W29:
      d_temp_callback = InterruptFunc<PinNum::W29>;
      break;
      
    case PinNum::Unknown:
      d_error = Error::Pin;
      d_err_msg = "Interrupt::Interrupt - The pin number is not set.";
      d_error_timer.singleShotZero();
      return;
    }
  
  // add the callback to the list, if the max interrupt instances have not been reached
  switch (d_mode)
    {
    case EdgeMode::Rising:
      s_callbacks[d_callback_index] = [this](){
	d_owner.call(&Interrupt::Owner::handleInterrupt,
		     this,
		     Vals(d_clock.now(),true));
      };
      break;
	  
    case EdgeMode::Falling:
      s_callbacks[d_callback_index] = [this](){
	d_owner.call(&Interrupt::Owner::handleInterrupt,
		     this,
		     Vals(d_clock.now(),false));
      };
      break;
	  
    case EdgeMode::Both:
      s_callbacks[d_callback_index] = [this](){
	d_owner.call(&Interrupt::Owner::handleInterrupt,
		     this,
		     Vals(d_clock.now(),readPin()));
      };
      break;
    }

  PIN_UTILS::setPullMode(d_pin, P_WP::PullMode::None);
  d_pin_state = digitalRead(static_cast<int>(d_pin));
}

//----------------------------------------------------------------------//
Interrupt::~Interrupt()
{
  s_callbacks.erase(d_callback_index);
}

//----------------------------------------------------------------------//
void Interrupt::start()
{
  if (d_started)
    {
      assert(false);
      d_owner.call(&Owner::handleError,
		   this,
		   Error::AlreadyStarted,
		   "Interrupt::start - Failed to start when already started.");
      return;
    }
  d_started = true;
  
  if (wiringPiISR(static_cast<int>(d_pin),
		  getWiringPiEdgeMode(d_mode),
		  *d_temp_callback.target<void(*)(void)>()) < 0)
    {
      s_callbacks.erase(d_callback_index);
      d_owner.call(&Owner::handleError,
		   this,
		   Error::Start,
		   "Interrupt::start - Failed to setup the WiringPi interrupt.");
      return;
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
int Interrupt::getWiringPiEdgeMode(EdgeMode mode)
{
  switch (mode)
    {
    case EdgeMode::Rising:
      return INT_EDGE_RISING;
	
    case EdgeMode::Falling:
      return INT_EDGE_FALLING;
      
    case EdgeMode::Both:
      return INT_EDGE_BOTH;
    }
  assert(false);
  std::ostringstream stream("Interrupt::getWiringPiEdgeMode - The mode value of '",
			    std::ios_base::app);
  stream << static_cast<int>(mode) << "' does not exist.";
  d_owner.call(&Owner::handleError,
	       this,
	       Error::EdgeMode,
	       stream.str());
}
