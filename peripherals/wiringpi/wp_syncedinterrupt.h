#ifndef PWP_SYNCEDINTERRUPT_H
#define PWP_SYNCEDINTERRUPT_H

#include "core_owner.h"
#include "utl_syncedthreadpipe.h"
#include "wp_interrupt.h"

namespace P_WP
{
  class SyncedInterrupt final : Interrupt::Owner
  {
  public:
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(SyncedInterrupt);
      virtual void handleInterrupt(SyncedInterrupt*,
				   Interrupt::Vals&&) = 0;
      virtual void handleError(SyncedInterrupt*,
			       Interrupt::Error,
			       const std::string& msg) = 0;
    };

  public:
    SyncedInterrupt(Owner*,
		    PinNum,
		    Interrupt::EdgeMode);

    SET_OWNER();

    void start();
    void stop();

  private:
    void handleInterrupt(Interrupt*,
			 Interrupt::Vals&&) override;
    void handleError(Interrupt*,
		     Interrupt::Error,
		     const std::string&) override;
    
  private:
    CORE::Owner<Owner> d_owner;
    std::unique_ptr<Interrupt> d_interrupt;
    std::unique_ptr<UTIL::SyncedThreadPipe<Interrupt::Vals>> d_pipe;
  };
};

#endif
