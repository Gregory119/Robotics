#ifndef CRC_STATEPWMREADER_H
#define CRC_STATEPWMREADER_H

#include "core_owner.h"

#include "crc_pwmreader.h"

namespace C_RC
{
  using PwmReaderType = PwmReader<unsigned>;
  
  class StatePwmReader final : PwmReaderType::Owner
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

    // percentage is of the range
    void setDetectPercentage(unsigned);
    
  private:
    void handleValue(PwmReaderType*, unsigned) override;
    void handleValueOutOfRange(PwmReaderType*, unsigned) override;
    void handleError(PwmReaderType*,
		     PwmReaderError,
		     const std::string& msg) override;

  private:
    bool isValHigh(unsigned val);
    bool isValLow(unsigned val);
    
  private:
    CORE::Owner<StatePwmReader::Owner> d_owner;
    std::unique_ptr<PwmReaderType> d_reader;

    unsigned d_detect_perc = 15;
    bool d_state = false;
    bool d_first_val = true;
  };
};

#endif
