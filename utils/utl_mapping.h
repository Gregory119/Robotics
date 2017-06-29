#ifndef UTIL_MAPPPING_H
#define UTIL_MAPPPING_H

#include <type_traits>
#include <cassert>

namespace UTIL
{
  struct Map
  {
    Map();
  Map(int in_max, int in_min, int o_max, int o_min)
  : d_in_max(in_max),
      d_in_min(in_min),
      d_out_max(o_max),
      d_out_min(o_min)
    {}
    Map(const Map &copy) = default;
    Map& operator=(const Map &copy) = default;

    int d_in_max = 0;
    int d_in_min = 0;
    int d_out_max = 0;
    int d_out_min = 0;
  };
  
  //----------------------------------------------------------------------//
  //flip axis will make the out max value correspond to the input min value and make the out min value correspond to the input max value.
  //make sure the input value is in range of its input max and min values
  // Only for fundamental types
  template <typename Tin, typename Tout>
    void mapFromTo(const Map &map, Tin in_val, Tout& out_val,  bool flip_axis=false)
    {
      assert(std::is_fundamental<Tin>::value &&
	     std::is_fundamental<Tout>::value);
      
      out_val = (static_cast<Tout>(in_val) - map.d_in_min)/ //use cast to induce up cast if necessary
	(map.d_in_max - map.d_in_min)*
	(map.d_out_max - map.d_out_min) + map.d_out_min;

      if (flip_axis)
	{
	  out_val = (map.d_out_max - out_val) + map.d_out_min;
	}
    }
};
#endif
