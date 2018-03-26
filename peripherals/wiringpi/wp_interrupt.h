#ifndef PWP_INTERRUPT_H
#define PWP_INTERRUPT_H

#include "wp_pins.h"

#include "core_owner.h"
#include "kn_asiocallbacktimer.h"

#include <chrono>
#include <functional>

namespace P_WP
{
  // The callback is internally called from a thread so use thread safe methods
  class Interrupt final
  {
  public:
    enum class EdgeMode
    {
      Rising,
	Falling,
	Both
    };

    enum class Error
    {
      EdgeMode,
	SetupInterrupt,
	MaxInstances,
	DuplicatePin,
	Pin,
	None
    };

    struct Vals
    {
      Vals() = default;
    Vals(std::chrono::time_point<std::chrono::steady_clock> t_point,
	 bool state)
    : time_point(std::move(t_point)),
	pin_state(state)
      {}
      
      std::chrono::time_point<std::chrono::steady_clock> time_point;
      bool pin_state = false;
    };
    
  public:
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(Interrupt);
      // this function is called from a thread
      virtual void handleInterrupt(Interrupt*,
				   Vals) = 0;
      virtual void handleError(Interrupt*, Error, const std::string&) = 0;
    };
    
  public:
    // A failure in the constructor will be called on a zero timer
    Interrupt(Owner*,
	      PinNum,
	      EdgeMode);
    Interrupt(Interrupt&&) = delete;
    ~Interrupt();

    SET_OWNER();
    
    bool readPin(); // reads physical pin value and updates cached value
    bool getCachedPinState();
    
    PinNum getPinNum() { return d_pin; }
    EdgeMode getEdgeMode() { return d_mode; }

  private:
    int getWiringPiEdgeMode(EdgeMode);

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
    
    const PinNum d_pin = PinNum::Unknown;
    EdgeMode d_mode = EdgeMode::Both;
    bool d_pin_state = false;
    const int d_callback_index = -1;

    Error d_error = Error::None;
    std::string d_err_msg;
    KERN::AsioCallbackTimer d_error_timer = KERN::AsioCallbackTimer("P_WP::Interrupt - Error timer.");

    std::chrono::steady_clock d_clock;
  };
};

#endif
