#ifndef CRC_STATEPWMREADER_H
#define CRC_STATEPWMREADER_H

#include "core_owner.h"

#include "crc_pwmreader.h"
#include "crc_pwmstatechangedetector.h"

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
    StatePwmReader(Owner*, P_WP::PinNum, PwmLimitsType);
    StatePwmReader(StatePwmReader&&) = delete;
    
    SET_OWNER();

  private:
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
