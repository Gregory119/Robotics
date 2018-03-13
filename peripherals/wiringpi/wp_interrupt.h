#ifndef PWP_INTERRUPT_H
#define PWP_INTERRUPT_H

#include "wp_pins.h"

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
    Interrupt(PinNum, Mode, std::function<void()> callback);
    Interrupt(Interrupt&&) = delete;

    // should be checked after construction
    bool hasError() { return d_has_error; }

    bool readPin(); // reads physical pin value and updates cached value
    bool getCachedPinState();
    
    PinNum getPinNum() { return d_pin; }
    Mode getMode() { return d_mode; }
    
  private:
    PinNum d_pin = PinNum::Unknown;
    Mode d_mode = Mode::Both;
    bool d_has_error = false;
    bool d_pin_state = false;
  };
};

#endif
