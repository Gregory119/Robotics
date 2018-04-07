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
void GoPro::ownerCommandFailed(GoPro::Cmd cmd,
			       GoPro::Error err,
			       const std::string& msg)
{
  d_owner->handleCommandFailed(this, cmd, err, msg);
}

//----------------------------------------------------------------------//
void GoPro::ownerCommandSuccessful(GoPro::Cmd cmd)
{
  d_owner->handleCommandSuccessful(this, cmd);
}

//----------------------------------------------------------------------//
void GoPro::ownerStreamDown()
{
  d_owner->handleStreamDown(this);
}
