#include "utl_mapping.h"

using namespace UTIL;

//----------------------------------------------------------------------//
Map::Map(double in_max, double in_min, double o_max, double o_min)
  : Map(in_max, in_min, o_max, o_min, (o_max+o_min)/2)
{}

//----------------------------------------------------------------------//
void Map::setInputRange(double in_max,
			double in_min,
			double in_mid)
{
  assert(in_max > in_min);
  assert(in_mid > in_min);
  assert(in_mid < in_max);
  d_in_max = in_max;
  d_in_min = in_min;
  d_in_mid = in_mid;
}

//----------------------------------------------------------------------//
void Map::setOutputRange(double out_max,
			 double out_min,
			 double out_mid)
{
  assert(out_max > out_min);
  assert(out_mid > out_min);
  assert(out_mid < out_max);
  d_out_max = out_max;
  d_out_min = out_min;
  d_out_mid = out_mid;
}
