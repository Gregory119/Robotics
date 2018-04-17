#include "wp_syncedinterrupt.h"

using namespace P_WP;

//----------------------------------------------------------------------//
SyncedInterrupt::SyncedInterrupt(Owner* o,
				 PinNum pin,
				 Interrupt::EdgeMode mode)
  : d_owner(o),
    d_interrupt(new Interrupt(this, pin, mode))
{}

//----------------------------------------------------------------------//
void SyncedInterrupt::start()
{
  if (d_pipe.get() == nullptr)
    {
      d_pipe.reset(new UTIL::SyncedThreadPipe<Interrupt::Vals>([this](Interrupt::Vals&& vals){
	    d_owner.call(&Owner::handleInterrupt, this, std::move(vals));
	  }));
    }
  d_interrupt->start();
}

//----------------------------------------------------------------------//
void SyncedInterrupt::stop()
{
  d_interrupt->stop();
  d_pipe.reset();
}

//----------------------------------------------------------------------//
void SyncedInterrupt::handleInterrupt(Interrupt*,
				      Interrupt::Vals&& vals)
{
  // Called by thread, so syncronize with a syncronized pipe.
  if (d_pipe.get()!=nullptr)
    {
      d_pipe->pushBack(std::move(vals));
    }
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
