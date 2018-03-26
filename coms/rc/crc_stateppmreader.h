#ifndef CRC_PPMREADER_H
#define CRC_PPMREADER_H

#include "crc_ppmreader.h"
#include "crc_pwmstatechangedetector.h"

namespace C_RC
{
  // Multiple observers can exist with the same channel.
  
  class StatePpmReader final : PpmReader::Owner
  {
    class Owner : PpmReader::Owner
    {
      OWNER_SPECIAL_MEMBERS(StatePpmReader);
      virtual void handleError(StatePpmReader*,
			       PpmReaderError,
			       const std::string& msg) = 0;
    };
      
    class Observer : PpmReader::Observer
    {
      OBSERVER_SPECIAL_MEMBERS(StatePpmReader);
      virtual void handleState(PpmReader<unsigned>*, bool state) = 0;

    private:
      void handleValue(PpmReader<unsigned>*, unsigned) override;
      void handleValueOutOfRange(PpmReader<unsigned>*, unsigned) override;

    private:
      std::unique_ptr<PwmStateChangeDetector<unsigned>> d_state_detector;
    };
    
  public:
    StatePpmReader(Owner*, P_WP::PinNum, PwmLimitsType);
    StatePpmReader(StatePpmReader&&) = delete;

    SET_OWNER();

    void attachObserver(Observer*);
    
  private:
    // PpmReader::Owner
    void handleError(PpmReader<unsigned>*,
		     PpmReaderError,
		     const std::string& msg) override;

  private:
    CORE::Owner<Owner> d_owner;
    std::unique_ptr<PpmReader> d_reader;
  };

  //----------------------------------------------------------------------//
  // StatePpmReader::Observer
  //----------------------------------------------------------------------//
  void StatePpmReader::Observer::handleValue(PpmReader<unsigned>* ppm_reader,
					     unsigned val)
  {
    if (d_state_detector == nullptr)
      {
	d_state_detector.reset(new PwmStateChangeDetector<unsigned>(ppm_reader->getMaxVal(),
								    ppm_reader->getMinVal(),
								    [&](bool state){
								      handleState(ppm_reader, state);
								    }));
      }
    d_state_detector->checkValue(val);
  }

  //----------------------------------------------------------------------//
  void StatePpmReader::Observer::handleValueOutOfRange(PpmReader<unsigned>* ppm_reader,
						       unsigned)
  {
    assert(false);
    ppm_reader->capData();
  }

  //----------------------------------------------------------------------//
  // StatePpmReader
  //----------------------------------------------------------------------//
  StatePpmReader::StatePpmReader(Owner* o,
				 P_WP::PinNum pin,
				 PwmLimitsType type)
    : d_owner(o),
    d_reader(new PpmReader<unsigned>(this,
				     pin,
				     PwmLimits<unsigned>::create(type, 2000, 0)))
      {
	d_reader->capData();
      }

  //----------------------------------------------------------------------//
  void StatePpmReader::attachObserver(Observer* ob)
  {
    d_reader->attachObserver(ob);
  }
  
  //----------------------------------------------------------------------//
  void StatePpmReader::handleError(PpmReader<unsigned>* reader,
				   PpmReaderError e,
				   const std::string& msg)
  {
    std::string new_msg = "C_RC::StatePpmReader::handleError - Ppm reader error: \n";
    new_msg += msg;
    d_owner.call(&Owner::handleError, reader, e, new_msg);
  }
};

#endif
