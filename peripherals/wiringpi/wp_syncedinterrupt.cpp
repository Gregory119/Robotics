#include "wp_syncedinterrupt.h"

using namespace P_WP;

//----------------------------------------------------------------------//
SyncedInterrupt::SyncedInterrupt(Owner* o,
				 PinNum pin,
				 Interrupt::EdgeMode mode)
  : d_owner(o),
    d_interrupt(new Interrupt(this, pin, mode)),
    d_pipe(new UTIL::SyncedThreadPipe<Interrupt::Vals>([this](Interrupt::Vals vals){
	  d_owner.call(&Owner::handleInterrupt, this, std::move(vals));
	}))
{}

//----------------------------------------------------------------------//
void SyncedInterrupt::handleInterrupt(Interrupt*,
				      Interrupt::Vals vals)
{
  // Called by thread, so syncronize with a syncronized pipe.
  d_pipe->pushBack(std::move(vals));
}

//----------------------------------------------------------------------//
void SyncedInterrupt::handleError(Interrupt*,
				  Interrupt::Error e,
				  const std::string& msg)
{
  std::string new_msg = "P_WP::SyncedInterrupt::handleError - There was an interrupt error with the following message:\n";
  new_msg += msg;
  d_owner.call(&Owner::handleError,this, e, new_msg);
}
