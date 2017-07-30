#include "utl_mapping.h"

using namespace UTIL;

//----------------------------------------------------------------------//
Map::Map(double in_max, double in_min, double o_max, double o_min)
  : Map(in_max, in_min, o_max, o_min, (o_max+o_min)/2)
{}

//----------------------------------------------------------------------//
void Map::setInputRange(double in_max, double in_min)
{
  assert(in_max > in_min);
  d_in_max = in_max;
  d_in_min = in_min;
}

//----------------------------------------------------------------------//
void Map::setOutputRange(double out_max, double out_min)
{
  assert(out_max > out_min);
  d_out_max = out_max;
  d_out_min = out_min;
}

//----------------------------------------------------------------------//
void Map::setOutputMiddle(double out_mid)
{
  assert(out_mid > d_out_min);
  assert(out_mid < d_out_max);
  d_out_mid = out_mid;
}
