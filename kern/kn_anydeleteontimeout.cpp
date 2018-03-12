#include "kn_anydeleteontimeout.h"

using namespace KERN;

//----------------------------------------------------------------------//
AnyDeleteOnTimeout::AnyDeleteOnTimeout()
{
  d_timer.setTimeoutCallback([this](){
      d_base_deleters.clear();
    });
}
