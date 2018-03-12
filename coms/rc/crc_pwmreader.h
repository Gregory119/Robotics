#ifndef CRC_PWMREADER_H
#define CRC_PWMREADER_H

#include "core_owner.h"
#include "core_threadpipe.h"

#include "crc_pwmutils.h"

#include "kn_safecallbacktimer.h"
#include "wp_interrupt.h"

#include <chrono>

namespace C_RC
{
  template <class T>
  class PwmReader final
  {
  public:
    enum class Error
    {
      InternalInterruptSetup,
	InvalidDuration
    };
    
  public:
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(PwmReader);
      // this will only be called when a new duration has been measured
      virtual void handleValue(PwmReader*, T) = 0;
      virtual void handleError(PwmReader*, Error, const std::string& msg) = 0;
    };
    
  public:
    PwmReader(Owner*, P_WP::PinNum, PwmLimits<T>);
    PwmReader(PwmReader&&) = delete;

    void setOwner(Owner* o) { d_owner = o; }

  private:
    void ownerPulseDuration();
    void processInterrupt();
    void ownerError(Error, const std::string& msg);
    
  private:
    CORE::Owner<PwmReader> d_owner;
    std::unique_ptr<P_WP::Interrupt> d_interrupt;

    PwmMap<T> d_map;
    
    CORE::ThreadPipe<std::chrono::microseconds> d_pulse_durations;
    std::chrono::steady_clock d_clock;
    std::chrono::time_point<std::chrono::steady_clock> d_pulse_start_pt;

    KERN::SafeCallbackTimer d_update_timer = KERN::SafeCallbackTimer("C_RC::PwmReader Update Timer");
    KERN::SafeCallbackTimer d_error_timer = KERN::SafeCallbackTimer("C_RC::PwmReader Zero Timer");
  };
};

#endif
