#include "common.h"
#include "assert.h"

using namespace JS_CO;

//----------------------------------------------------------------------//
int mapFromTo(const Map &map, int orig_val, bool flip_axis)
{
  //check value is in range
  assert(orig_val >= map.orig_min);
  assert(orig_val <= map.orig_max);

  int new_val = (orig_val - map.orig_min)/(map.orig_max - map.orig_min)*
    (map.new_max - map.new_min) + map.new_min;

  if (flip_axis)
    {
      new_val = (map.new_max - new_val) + map.new_min;
    }

  return new_val;
}
