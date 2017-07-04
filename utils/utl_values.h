#ifndef UTIL_VALUES_H
#define UTIL_VALUES_H

#include <cassert>

namespace UTIL
{
  enum class Sign
    {
      Pos,
      Neg,
      Zero
    };
  
  template <class T>
  class SignCheck final
  {
  public:
    SignCheck(const T& zero_ref = 0)
      {
	assert(std::is_fundamental<T>::value);
      }
    
    Sign getSign(const T& val)
    {
      if (val > d_zero_ref)
	{
	  return Sign::Pos;
	}
      else if (val < d_zero_ref)
	{
	  return Sign::Neg;
	}

      return Sign::Zero;
    }

  private:
    T d_zero_ref = 0;
  };
};

#endif
