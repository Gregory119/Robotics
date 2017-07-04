#include "utl_mapping.h"

using namespace UTIL;

//----------------------------------------------------------------------//
void Map::setInputRange(double in_max, double in_min)
{
  d_in_max = in_max;
  d_in_min = in_min;
}

//----------------------------------------------------------------------//
void Map::setOutputRange(double out_max, double out_min)
{
  d_out_max = out_max;
  d_out_min = out_min;
}
