#include "dgp_gopro.h"

#include <cassert>

using namespace D_GP;

//----------------------------------------------------------------------//
GoPro::GoPro(GoPro::Owner* o)
{
  assert(o!=nullptr);
  d_owner = o;
}
