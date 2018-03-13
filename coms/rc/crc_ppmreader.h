#ifndef CRC_PPMREADER_H
#define CRC_PPMREADER_H

#include "core_owner.h"
#include "crc_pwmutils.h"
#include "kn_safecallbacktimer.h"
#include "wp_interrupt.h"

#include <chrono>
#include <list>

namespace C_RC
{
  template <class T>
  class PpmReader final
  {
  public:
    enum class Error
    {
      InternalInterruptSetup,
	InvalidDuration
    };

    template <class Type>
    struct Value
    {
      const Type val = 0;
      const unsigned channel = 0;
    };
    
  public:
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(PpmReader);
      // this will only be called when a new duration has been measured
      virtual void handleValue(PpmReader*, const Value<T>&) = 0;
      virtual void handleError(PpmReader*, Error, const std::string& msg) = 0;
    };
    
  public:
    PpmReader(Owner*, P_WP::PinNum, PwmLimits<T>);
    PpmReader(PpmReader&&) = delete;

    SET_OWNER();

    // This will only fail if the number of channels has not been detected yet.
    bool getNumChannels(unsigned&) { return d_channels; }

  private:
    void processData();
    void ownerValue();
    void clearData();
    void processInterrupt();
    void ownerError(Error, const std::string& msg);

    struct Duration
    {
      Duration() = default;
    Duration(std::chrono::microseconds dur, unsigned ch)
    :   duration(std::move(dur)),
	channel(ch)
      {}
      std::chrono::microseconds duration;
      unsigned channel = 0;
    };
    
  private:
    CORE::Owner<PpmReader> d_owner;
    std::unique_ptr<P_WP::Interrupt> d_interrupt;

    PwmMap<T> d_map;
    unsigned d_channels = 0; // will be auto-detected
    unsigned d_current_channel = 0; // zero represents the start pulse
    unsigned d_count_conseq_diff_channels = 0;

    std::list<Duration> d_pulse_durations;
    std::chrono::steady_clock d_clock;
    std::chrono::time_point<std::chrono::steady_clock> d_pulse_start_pt;

    KERN::SafeCallbackTimer d_data_timer = KERN::SafeCallbackTimer("C_RC::PpmReader Data Timer");
    KERN::SafeCallbackTimer d_error_timer = KERN::SafeCallbackTimer("C_RC::PpmReader Zero Timer");

    std::mutex d_data_mutex;
  };
};

#endif
