#include "dgp_gopro.h"

#include <cassert>

using namespace D_GP;

//----------------------------------------------------------------------//
GoPro::GoPro(GoProOwner* o)
{
  assert(o!=nullptr);
  d_owner = o;
}
