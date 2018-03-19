#include "crc_statepwmreader.h"

#include <iostream>

using namespace C_RC;

//----------------------------------------------------------------------//
StatePwmReader::StatePwmReader(Owner* o,
			       P_WP::PinNum pin,
			       PwmLimitsType type)
  : d_owner(o),
    d_reader(new PwmReaderType(this,
			       pin,
			       PwmLimits<unsigned>::create(type, 2000, 0)))
{
  d_reader->capData();
}

//----------------------------------------------------------------------//
void StatePwmReader::setDetectPercentage(unsigned per)
{
  if (per > 100)
    {
      assert(false);
      return;
    }
  
  d_detect_perc = per;
}

//----------------------------------------------------------------------//
void StatePwmReader::handleValue(PwmReaderType*, unsigned val)
{
  if (d_first_val)
    {
      d_first_val = false;
      if (isValHigh(val))
	{
	  d_state = true;
	  d_owner.call(&StatePwmReader::Owner::handleState, this, d_state);
	  return;
	}

      if (isValLow(val))
	{
	  d_state = false;
	  d_owner.call(&StatePwmReader::Owner::handleState, this, d_state);
	  return;
	}
    }
  
  // change to high state
  if (isValHigh(val) && !d_state)
    {
      d_state = true;
      d_owner.call(&StatePwmReader::Owner::handleState, this, d_state);
      return;
    }

  // change to low state
  if (isValLow(val) && d_state)
    {
      d_state = false;
      d_owner.call(&StatePwmReader::Owner::handleState, this, d_state);
      return;
    }
}

//----------------------------------------------------------------------//
void StatePwmReader::handleValueOutOfRange(PwmReaderType*, unsigned)
{
  assert(false);
}

//----------------------------------------------------------------------//
void StatePwmReader::handleError(PwmReaderType*,
				 PwmReaderError e,
				 const std::string& msg)
{
  d_owner.call(&StatePwmReader::Owner::handleError, this, e, msg);
}

//----------------------------------------------------------------------//
bool StatePwmReader::isValHigh(unsigned val)
{
  unsigned up_diff = d_reader->getMaxVal() - val;
  unsigned up_diff_per = up_diff*100/d_reader->getValRange();

  // change to high state
  if (up_diff_per <= d_detect_perc)
    {
      return true;
    }
  return false;
}

//----------------------------------------------------------------------//
bool StatePwmReader::isValLow(unsigned val)
{
  unsigned low_diff = val - d_reader->getMinVal();
  unsigned low_diff_per = low_diff*100/d_reader->getValRange();

  // change to low state
  if (low_diff_per <= d_detect_perc)
    {
      return true;
    }
  return false;
}
