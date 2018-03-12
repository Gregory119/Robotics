#ifndef CRC_PPMREADER_H
#define CRC_PPMREADER_H

#include "core_mutexedmicrosec.h"
#include "core_owner.h"
#include "core_threadpipe.h"

#include "kn_safecallbacktimer.h"
#include "wp_interrupt.h"

#include <chrono>

namespace C_RC
{
  class PpmReader final
  {
  public:
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(PpmReader);
      // this will only be called when a new duration has been measured
      virtual void handlePulseDuration(PpmReader*,
				       unsigned channel,
				       const std::chrono::microseconds&) = 0;
      virtual void handleError(PpmReader*) = 0;
    };
    
  public:
    PpmReader(Owner*, P_WP::PinNum, );
    PpmReader(PpmReader&&) = delete;

    void setOwner(Owner*);

  private:
    void ownerPulseDuration();
    void processInterrupt();

  private:
    struct PulseDuration
    {
      std::chrono::microseconds duration;
      unsigned channel = 0;
    };
    
  private:
    std::atomic<Owner*> d_owner;
    std::unique_ptr<P_WP::Interrupt> d_interrupt;

    CORE::ThreadPipe<std::chrono::microseconds> d_pulse_durations;
    std::chrono::steady_clock d_clock;
    std::chrono::time_point<std::chrono::steady_clock> d_pulse_start_pt;
    std::chrono::time_point<std::chrono::steady_clock> d_pulse_end_pt;

    KERN::SafeCallbackTimer d_update_timer = KERN::SafeCallbackTimer("C_RC::PpmReader Update Timer");
    KERN::AsioCallbackTimer d_zero_timer = KERN::AsioCallbackTimer("C_RC::PpmReader Zero Timer");
  };
};

#endif
