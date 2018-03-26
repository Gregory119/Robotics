#include "wp_pins.h"

#include <wiringPi.h>

using namespace P_WP;

//----------------------------------------------------------------------//
void PIN_UTILS::setPullMode(int pin, PullMode pull)
{
  if (!PIN_UTILS::isNumInPinRange(pin))
    {
      assert(false);
      return;
    }
  
  PIN_UTILS::setPullMode(static_cast<PinNum>(pin), pull);
}

//----------------------------------------------------------------------//
void PIN_UTILS::setPullMode(PinNum pin, PullMode pull)
{
  switch (pull)
    {
    case PullMode::Up:
      pullUpDnControl(static_cast<int>(pin), PUD_UP);
      break;
      
    case PullMode::Down:
      pullUpDnControl(static_cast<int>(pin), PUD_DOWN);
      break;

    case PullMode::None:
      pullUpDnControl(static_cast<int>(pin), PUD_OFF);
      break;

    default:
      assert(false);
      break;
    }
}

//----------------------------------------------------------------------//
bool PIN_UTILS::isNumInPinRange(int num)
{
  PinNum pin = static_cast<PinNum>(num);

  switch (pin)
    {
    case PinNum::W8:
    case PinNum::W9:
    case PinNum::W7:
    case PinNum::W15:
    case PinNum::W16:
    case PinNum::W0:
    case PinNum::W1:
    case PinNum::W2:
    case PinNum::W3:
    case PinNum::W4:
    case PinNum::W5:
    case PinNum::W12:
    case PinNum::W13:
    case PinNum::W6:
    case PinNum::W14:
    case PinNum::W10:
    case PinNum::W11:
    case PinNum::W30:
    case PinNum::W31:
    case PinNum::W21:
    case PinNum::W22:
    case PinNum::W26:
    case PinNum::W23:
    case PinNum::W24:
    case PinNum::W27:
    case PinNum::W25:
    case PinNum::W28:
    case PinNum::W29:
      return true;
	
    case PinNum::Unknown:
      break;
    }
  return false;
}
