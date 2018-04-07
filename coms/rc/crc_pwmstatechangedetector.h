#ifndef CRC_PWMSTATECHANGEDETECTOR_H
#define CRC_PWMSTATECHANGEDETECTOR_H

#include "crc_pwmstatechecker.h"

#include <functional>
#include <memory>

namespace C_RC
{
  template <class T>
    class PwmStateChangeDetector final
    {
    public:
      // The callback is called when a changed state has been detected
      PwmStateChangeDetector(T max_val,
			     T min_val,
			     std::function<void(bool)> callback);

      // Must be called with every new value
      void checkValue(T);
      
    private:
      std::unique_ptr<PwmStateChecker<T>> d_state_checker;
      std::function<void(bool)> d_callback;

      bool d_state = false;
      bool d_first_val = true;
    };

  //----------------------------------------------------------------------//
  template <class T>
    PwmStateChangeDetector<T>::PwmStateChangeDetector(T max_val,
						      T min_val,
						      std::function<void(bool)> callback)
    : d_state_checker(new PwmStateChecker<T>(max_val, min_val)),
    d_callback(callback)
    {
      if (!std::is_fundamental<T>::value)
	{
	  assert(false);
	  //LOG!!!
	  return;
	}
    }

  //----------------------------------------------------------------------//
  template <class T>
    void PwmStateChangeDetector<T>::checkValue(T val)
    {
      assert(d_callback);
      
      if (d_first_val)
	{
	  d_first_val = false;
	  if (d_state_checker->isHigh(val))
	    {
	      d_state = true;
	      return;
	    }

	  if (d_state_checker->isLow(val))
	    {
	      d_state = false;
	      return;
	    }
	}
  
      // change to high state
      if (d_state_checker->isHigh(val) && !d_state)
	{
	  d_state = true;
	  d_callback(d_state);
	  return;
	}

      // change to low state
      if (d_state_checker->isLow(val) && d_state)
	{
	  d_state = false;
	  d_callback(d_state);
	  return;
	}
    }
};

#endif
