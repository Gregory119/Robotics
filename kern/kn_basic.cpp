#include "kn_basic.h"

#include <cassert>

using namespace KERN;

//----------------------------------------------------------------------//
void KernBasic::run()
{
  assert(d_comp != nullptr);

  while (d_comp->process()) {}
}
