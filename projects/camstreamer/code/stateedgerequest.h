#ifndef STATEEDGEREQUEST_H
#define STATEEDGEREQUEST_H

#include "types.h"

#include "core_owner.h"

class StateEdgeRequest
{
 public:
  enum class DurationTrigger
  {
    None,
      AfterDuration,
      OnOffBeforeDuration
  }
  
 public:
  class Owner
  {
    OWNER_SPECIAL_MEMBERS(StateEdgeRequest);
    virtual void handleRequest(StateEdgeRequest*, DurationTrigger) = 0;
    virtual void handleError(StateEdgeRequest*, const std::string&) = 0;
  };
  
 public:
  virtual ~StateEdgeRequest() = default;
  StateEdgeRequest(StateEdgeRequest&&) = delete;

  SET_OWNER();

  // This class will trigger by default to a high and low state.
  // This function activates a timer to check if the trigger state goes on and off before it ends.
  void activateDurationTriggers(const std::chrono::milliseconds& dur,
				bool trig_state);
  
 protected:
 StateEdgeRequest(Owner* o) // ensure it is only instantiated by a child class
   : d_owner(o)
    {
      assert(o != nullptr);
    }

  void processState(bool); // call this with the new state
  
 protected:
  CORE::Owner<Owner> d_owner;
  KERN::AsioCallbackTimer d_error_timer("StateEdgeRequest - Error timer.");
  
 private:
  bool d_has_dur_trig = false;
  bool d_trig_state = true;
  bool d_on_off_wait_set = false;
  KERN::AsioCallbackTimer d_state_timer("StateEdgeRequest - State timer.");
  
  std::string d_err_msg;
};

#endif
