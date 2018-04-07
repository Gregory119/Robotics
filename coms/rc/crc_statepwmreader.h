#ifndef CRC_STATEPWMREADER_H
#define CRC_STATEPWMREADER_H

#include "core_owner.h"

#include "crc_pwmreader.h"
#include "crc_pwmstatechangedetector.h"

#include <chrono>

namespace C_RC
{
  class StatePwmReader final : PwmReader<unsigned>::Owner
  {
  public:
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(StatePwmReader);
      virtual void handleState(StatePwmReader*, bool) = 0;
      virtual void handleError(StatePwmReader*,
			       PwmReaderError,
			       const std::string& msg) = 0;
    };

  public:
    // the PwmLimits max and min vals are changed internally, so any number can be used for these
    StatePwmReader(Owner*,
		   P_WP::PinNum,
		   std::chrono::microseconds max_pulse_duration,
		   std::chrono::microseconds min_pulse_duration);
    StatePwmReader(StatePwmReader&&) = delete;
    
    SET_OWNER();

    void start();

  private:
    // PwmReader<unsigned>::Owner
    void handleValue(PwmReader<unsigned>*, unsigned) override;
    void handleValueOutOfRange(PwmReader<unsigned>*, unsigned) override;
    void handleError(PwmReader<unsigned>*,
		     PwmReaderError,
		     const std::string& msg) override;

  private:
    CORE::Owner<StatePwmReader::Owner> d_owner;
    std::unique_ptr<PwmReader<unsigned>> d_reader;

    std::unique_ptr<PwmStateChangeDetector<unsigned>> d_state_detector;
  };
};

#endif
