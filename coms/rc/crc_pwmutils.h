#ifndef CRC_PWMUTILS_H
#define CRC_PWMUTILS_H

#include "utl_mapping.h"

#include <chrono>
#include <utility>

namespace C_RC
{
  const std::chrono::microseconds& getMaxPwmDuration();
  const std::chrono::microseconds& getMinPwmDuration();

  enum class PwmLimitsType
  {
    Narrow,
      Wide,
      PpmFlySky
      };

  
  
  template <class T>
  struct PwmLimits
  {
    PwmLimits(T max_v,
	      T min_v,
	      std::chrono::microseconds max_dur,
	      std::chrono::microseconds min_dur);
    PwmLimits(const PwmLimits&) = default;
    PwmLimits& operator=(const PwmLimits&) = default;
    PwmLimits(PwmLimits&&) = default;
    PwmLimits& operator=(PwmLimits&&) = default;
    virtual ~PwmLimits() = default;

    bool isDurationValid(const std::chrono::microseconds&);
    bool isValueValid(T);
    bool capDuration(const std::chrono::microseconds&, std::chrono::microseconds&);

    static PwmLimits<T> create(PwmLimitsType, T max_val, T min_val);
    
    const T max_val = 0;
    const T min_val = 0;
    const T val_range = 0;
    const unsigned max_duration_micros = 0;
    const unsigned min_duration_micros = 0;
    const unsigned duration_range_micros = 0;
  };
  
  template <class T>
  class PwmMap
  {
  public:
    PwmMap(const PwmLimits<T>& limits);
    PwmMap(PwmMap&&) = default;
    PwmMap& operator=(PwmMap&&) = default;
    virtual ~PwmMap() = default;
    
    std::chrono::microseconds getDuration(T val);
    T getValue(const std::chrono::microseconds&);
    bool isDurationValid(const std::chrono::microseconds&);
    bool isValueValid(T);
    bool capDuration(const std::chrono::microseconds&, std::chrono::microseconds&);
    const PwmLimits<T>& getPwmLimits() const { return d_limits; }

  private:
    UTIL::Map d_map_pulse_to_val;
    PwmLimits<T> d_limits;
  };

  //----------------------------------------------------------------------//
  // PwmLimits
  //----------------------------------------------------------------------//
  template <class T>
    PwmLimits<T>::PwmLimits(T max_v,
			    T min_v,
			    std::chrono::microseconds max_dur,
			    std::chrono::microseconds min_dur)
    : max_val(max_v),
    min_val(min_v),
    val_range(max_val-min_val),
    max_duration_micros(max_dur.count()),
    min_duration_micros(min_dur.count()),
    duration_range_micros(max_duration_micros - min_duration_micros)
      {
	if (!std::is_fundamental<T>::value)
	  {
	    // LOG
	    assert(false);
	    return;
	  }

	if (max_val < min_val)
	  {
	    // LOG
	    assert(false);
	    return;
	  }
	
	if (max_dur.count() > 3000 ||
	    max_dur.count() < min_dur.count()) // implies requirement of min_dur.count() >= 0
	  {
	    // LOG
	    assert(false);
	    return;
	  }
      }

  //----------------------------------------------------------------------//
  template <class T>
    bool PwmLimits<T>::isDurationValid(const std::chrono::microseconds& dur)
    {
      return dur.count() > max_duration_micros ||
	min_duration_micros > dur.count();
    }

  //----------------------------------------------------------------------//
  template <class T>
    bool PwmLimits<T>::isValueValid(T val)
    {
      return !((val > max_val) || (val < min_val));
    }

  //----------------------------------------------------------------------//
  template <class T>
    bool PwmLimits<T>::capDuration(const std::chrono::microseconds& in,
				   std::chrono::microseconds& out)
    {
      if (in.count() > max_duration_micros)
	{
	  if ((in.count() - max_duration_micros)*100/duration_range_micros > 30)
	    {
	      return false;
	    }
	  out = std::chrono::microseconds(max_duration_micros);
	  return true;
	}
      
      if (in.count() < min_duration_micros)
	{
	  if ((min_duration_micros - in.count())*100/duration_range_micros > 30)
	    {
	      return false;
	    }
	  out = std::chrono::microseconds(min_duration_micros);
	  return true;
	}

      out = in;
      return true;
    }

  //----------------------------------------------------------------------//
  template <class T>
    PwmLimits<T> PwmLimits<T>::create(PwmLimitsType type, T max_val, T min_val)
  {
    switch (type)
      {
      case PwmLimitsType::Narrow:
	return PwmLimits<T>(max_val,
			    min_val,
			    std::chrono::microseconds(2000),
			    std::chrono::microseconds(1000));

      case PwmLimitsType::Wide:
	return PwmLimits<T>(max_val,
			    min_val,
			    std::chrono::microseconds(2500),
			    std::chrono::microseconds(500));

      case PwmLimitsType::PpmFlySky:
	return PwmLimits<T>(max_val,
			    min_val,
			    std::chrono::microseconds(2000),
			    std::chrono::microseconds(1000));
      }
    assert(false);
    return PwmLimits<T>(max_val,
			min_val,
			std::chrono::microseconds(2000),
			std::chrono::microseconds(1000));
  }
  
  //----------------------------------------------------------------------//
  // PwmMap
  //----------------------------------------------------------------------//
  template <class T>
    PwmMap<T>::PwmMap(const PwmLimits<T>& limits)
    : d_limits(limits)
    {
      if (!std::is_fundamental<T>::value)
	{
	  assert(false);
	  return;
	}
  
      d_map_pulse_to_val.setInputRange(d_limits.max_duration_micros,
				       d_limits.min_duration_micros,
				       (d_limits.max_duration_micros+d_limits.min_duration_micros)/2);
      d_map_pulse_to_val.setOutputRange(d_limits.max_val,
					d_limits.min_val,
					(d_limits.max_val+d_limits.min_val)/2);
    }

  //----------------------------------------------------------------------//
  template <class T>
    std::chrono::microseconds PwmMap<T>::getDuration(T val)
    {
      return std::chrono::microseconds(d_map_pulse_to_val.inverseMap<unsigned>(val));
    }

  //----------------------------------------------------------------------//
  template <class T>
    T PwmMap<T>::getValue(const std::chrono::microseconds& time)
    {
      return d_map_pulse_to_val.map<T>(time.count());
    }

  //----------------------------------------------------------------------//
  template <class T>
    bool PwmMap<T>::isDurationValid(const std::chrono::microseconds& dur)
    {
      return d_limits.isDurationValid(dur);
    }

  //----------------------------------------------------------------------//
  template <class T>
    bool PwmMap<T>::isValueValid(T val)
    {
      return d_limits.isValueValid(val);
    }

  //----------------------------------------------------------------------//
  template <class T>
    bool PwmMap<T>::capDuration(const std::chrono::microseconds& in,
			   std::chrono::microseconds& out) 
    {
      return d_limits.capDuration(in, out);
    }
};

#endif
