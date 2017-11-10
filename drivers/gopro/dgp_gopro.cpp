#include "dgp_gopro.h"

#include <cassert>

using namespace D_GP;

//----------------------------------------------------------------------//
GoPro::GoPro(GoPro::Owner* o)
{
  assert(o!=nullptr);
  d_owner = o;
}

//----------------------------------------------------------------------//
void GoPro::ownerHandleCommandFailed(GoPro::Cmd cmd, GoPro::Error err)
{
  d_owner->handleCommandFailed(this, cmd, err);
}

//----------------------------------------------------------------------//
void GoPro::ownerHandleCommandSuccessful(GoPro::Cmd cmd)
{
  d_owner->handleCommandSuccessful(this, cmd);
}

//----------------------------------------------------------------------//
void GoPro::ownerHandleStreamDown()
{
  d_owner->handleStreamDown(this);
}
