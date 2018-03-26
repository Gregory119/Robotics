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
  Sign getSign(const T& val)
  {
    assert(std::is_fundamental<T>::value);
    if (val > 0)
      {
	return Sign::Pos;
      }
    else if (val < 0)
      {
	return Sign::Neg;
      }

    return Sign::Zero;
  }
};

#endif
