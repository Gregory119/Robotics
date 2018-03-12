#ifndef CRC_PWMUTILS_H
#define CRC_PWMUTILS_H

#include "utl_mapping.h"

#include <chrono>
#include <utility>

namespace C_RC
{
  template <class T>
  struct PwmLimits
  {
    PwmLimits(T max_v,
	      T min_v,
	      std::chrono::microseconds max_dur,
	      std::chrono::microseconds min_dur);
    PwmLimits(PwmLimits&&) = default;
    PwmLimits& operator=(PwmLimits&&) = default;
    virtual ~PwmLimits();

    bool isDurationValid(const std::chrono::microseconds&);
    bool isValueValid(T);
    
    const T max_val;
    const T min_val;
    const unsigned max_duration_micros = 0;
    const unsigned min_duration_micros = 0;
  };

  template <class T>
  struct PwmNarrowLimits : public PwmLimits<T>
  {
  PwmNarrowLimits(T max_v, T min_v)
    : PwmLimits<T>(std::move(max_v),
		   std::move(min_v),
		   2000,
		   1000)
      {}
  };

  template <class T>
  struct PwmWideLimits : public PwmLimits<T>
  {
  PwmWideLimits(T max_v, T min_v)
    : PwmLimits<T>(std::move(max_v),
		   std::move(min_v),
		   2500,
		   500)
      {}
  };

  template <class T>
  class PwmMap
  {
  public:
    PwmMap(PwmLimits<T> limits);
    PwmMap(PwmLimits<T>&& limits);
    PwmMap(PwmMap&&) = default;
    virtual ~PwmMap() = default;
    PwmMap& operator=(PwmMap&&) = default;

    std::chrono::microseconds getDuration(T val);
    T getValue(const std::chrono::microseconds&);
    bool isDurationValid(const std::chrono::microseconds&);
    bool isValueValid(T);
    const PwmLimits<T>& getPwmLimits() const { return d_limits; }

  private:
    UTIL::Map d_map_pulse_to_val;
    PwmLimits<T> d_limits;
  };
};

#endif
