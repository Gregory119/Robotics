#ifndef CRC_STATEPPMREADER_H
#define CRC_STATEPPMREADER_H

#include "crc_ppmreader.h"
#include "crc_pwmstatechangedetector.h"

#include <chrono>

namespace C_RC
{
  // Multiple observers can exist with the same channel.
  
  class StatePpmReader final
  {
  public:
    class Observer : PpmReader<unsigned>::Observer
    {
      OBSERVER_SPECIAL_MEMBERS(StatePpmReader);
      virtual void handleState(PpmReader<unsigned>*, bool state) = 0;
      // Inherited pure virtuals:
      // handleError()
      // getChannel()

    private:
      void handleValue(PpmReader<unsigned>*, unsigned) override;
      void handleValueOutOfRange(PpmReader<unsigned>*, unsigned) override;

    private:
      std::unique_ptr<PwmStateChangeDetector<unsigned>> d_state_detector;
    };
    
  public:
    // the PwmLimits max and min vals are changed internally, so any number can be used for these
    StatePpmReader(P_WP::PinNum,
		   std::chrono::microseconds max_pulse_duration,
		   std::chrono::microseconds min_pulse_duration);
    StatePpmReader(StatePpmReader&&) = delete;

    void start();
    void startIfNotStarted() { d_reader->startIfNotStarted(); }
    void attachObserver(Observer*);
    
  private:
    std::unique_ptr<PpmReader<unsigned>> d_reader;
  };
};

#endif
