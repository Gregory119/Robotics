#ifndef PWP_INTERRUPT_H
#define PWP_INTERRUPT_H

#include "wp_pins.h"

#include "core_owner.h"

#include <functional>

namespace P_WP
{
  // The callback is internally called from a thread so use thread safe methods
  class Interrupt final
  {
  public:
    enum class Mode
    {
      Rising,
	Falling,
	Both
    };

  public:
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(Interrupt);
      virtual void handleInterrupt(Interrupt*) = 0;
    };
    
  public:
    Interrupt(Owner*,
	      PinNum,
	      Mode);
    Interrupt(Interrupt&&) = delete;

    SET_OWNER();
    
    // should be checked after construction
    bool hasError() { return d_has_error; }

    bool readPin(); // reads physical pin value and updates cached value
    bool getCachedPinState();
    
    PinNum getPinNum() { return d_pin; }
    Mode getMode() { return d_mode; }

  private:
    int getWiringPiMode(Mode);

  private:
    enum class FuncCount
    {
      One,
	Two,
	Three,
	Four,
	Five,
	Six,
	End
	};
    
  private:
    CORE::Owner<Interrupt::Owner> d_owner;
    
    PinNum d_pin = PinNum::Unknown;
    Mode d_mode = Mode::Both;
    bool d_has_error = false;
    bool d_pin_state = false;
  };
};

#endif
