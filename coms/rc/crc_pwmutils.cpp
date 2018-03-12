#include "crc_pwmutils.h"

#include <cassert>
#include <type_traits>

using namespace C_RC;

//----------------------------------------------------------------------//
template <class T>
PwmLimits<T>::PwmLimits(T max_v,
			T min_v,
			std::chrono::microseconds max_dur,
			std::chrono::microseconds min_dur)
  : max_val(std::move(max_v)),
    min_val(std::move(min_v)),
    max_duration_micros(std::move(max_dur.count())),
    min_duration_micros(std::move(min_dur.count()))
{
  if (!std::is_fundamental<T>::value)
    {
      assert(false);
      return;
    }
}

//----------------------------------------------------------------------//
template <class T>
bool PwmLimits<T>::isDurationValid(const std::chrono::microseconds& dur)
{
  return !((dur.count() > max_duration_micros) ||
	   (dur.count() < min_duration_micros));
}

//----------------------------------------------------------------------//
template <class T>
bool PwmLimits<T>::isValueValid(T val)
{
  return !((val > max_val) || (val < min_val));
}

//----------------------------------------------------------------------//
template <class T>
PwmMap<T>::PwmMap(PwmLimits<T> limits)
  : PwmMap<T>(std::move(limits))
{}

//----------------------------------------------------------------------//
template <class T>
PwmMap<T>::PwmMap(PwmLimits<T>&& limits)
  : d_limits(std::move(limits))
{
  if (!std::is_fundamental<T>::value)
    {
      assert(false);
      return;
    }
  
  d_map_pulse_to_val.setInputRange(d_limits.max_duration_micros,
				   d_limits.min_duration_micros);
  d_map_pulse_to_val.setOutputRange(d_limits.max_val,
				    d_limits.min_val);
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
  return !((dur.count() > d_limits.max_duration_micros) ||
	   (dur.count() < d_limits.min_duration_micros));
}

//----------------------------------------------------------------------//
template <class T>
bool PwmMap<T>::isValueValid(T val)
{
  return !((val > d_limits.max_val) ||
	   (val < d_limits.min_val));
}
