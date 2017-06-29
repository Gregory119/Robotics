#ifndef UTIL_VALUES_H
#define UTIL_VALUES_H

#include <cassert>

namespace UTIL
{
  namespace Values
  {
    enum Sign
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
	    return Pos;
	  }
	else if (val < 0)
	  {
	    return Neg;
	  }

	return Zero;
      }
  };
};

#endif
