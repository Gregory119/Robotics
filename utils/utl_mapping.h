#ifndef UTIL_MAPPPING_H
#define UTIL_MAPPPING_H

#include "assert.h"

namespace UTIL
{
  struct Map
  {
    Map();
  Map(int o_max, int o_min, int n_max, int n_min)
  : d_orig_max(o_max),
      d_orig_min(o_min),
      d_new_max(n_max),
      d_new_min(n_min)
    {}
    Map(const Map &copy) = default;
    Map& operator=(const Map &copy) = default;

    int d_orig_max = 0;
    int d_orig_min = 0;
    int d_new_max = 0;
    int d_new_min = 0;
  };
  
  //----------------------------------------------------------------------//
  //flip axis will make the new max value correspond to the original min value and make the new min value correspond to the original max value.
  //make sure the original value is in range of its original max and min values
  template <typename T>
    double mapFromTo(const Map &map, T orig_val, bool flip_axis=false)
    {
      double new_val = (static_cast<double>(orig_val) - map.d_orig_min)/
	(map.d_orig_max - map.d_orig_min)*
	(map.d_new_max - map.d_new_min) + map.d_new_min;

      if (flip_axis)
	{
	  new_val = (map.d_new_max - new_val) + map.d_new_min;
	}

      return new_val;
    }
};
#endif
