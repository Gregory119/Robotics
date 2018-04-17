#ifndef CRC_PWMSTATECHECKER_H
#define CRC_PWMSTATECHECKER_H

#include <type_traits>
#include <cassert>

namespace C_RC
{
  template <class T>
    class PwmStateChecker final
    {
    public:
      PwmStateChecker(T max_val, T min_val);

      // maximum percentage is 20%
      void setDetectRangePercent(unsigned);
      bool isHigh(T val);
      bool isLow(T val);

    private:
      unsigned d_detect_perc = 15;
      T d_max = 0;
      T d_min = 0;
      const unsigned d_range = 0;
    };

  template <class T>
    PwmStateChecker<T>::PwmStateChecker(T max_val, T min_val)
    : d_max(max_val),
    d_min(min_val),
    d_range(max_val-min_val)
    {
      if (!std::is_fundamental<T>::value)
	{
	  assert(false);
	  //LOG!!!
	  return;
	}

      assert(max_val > min_val);
    }

  template <class T>
    void PwmStateChecker<T>::setDetectRangePercent(unsigned per)
    {
      if (per > 20)
	{
	  assert(false);
	  return;
	}
  
      d_detect_perc = per;
    }

  template <class T>
    bool PwmStateChecker<T>::isHigh(T val)
    {
      unsigned high_diff = d_max - val;
      unsigned high_diff_per = high_diff*100/d_range;

      // change to high state
      if (high_diff_per <= d_detect_perc)
	{
	  return true;
	}
      return false;
    }

  template <class T>
    bool PwmStateChecker<T>::isLow(T val)
    {
      unsigned low_diff = val - d_min;
      unsigned low_diff_per = low_diff*100/d_range;

      // change to high state
      if (low_diff_per <= d_detect_perc)
	{
	  return true;
	}
      return false;
    }
};

#endif
