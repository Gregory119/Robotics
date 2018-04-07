#ifndef STATEEDGEREQUEST_H
#define STATEEDGEREQUEST_H

#include "types.h"
#include "core_owner.h"
#include "kn_asiocallbacktimer.h"

#include <chrono>

class StateEdgeRequest
{
 public:
  enum class TriggerSetting
  {
    None,
      Combination // either triggers with AfterDuration or OnOffBeforeDuration
  };
  
  enum class Trigger
  {
    None,
      AfterDuration,
      OnOffBeforeDuration
      };

 public:
  class Owner
  {
    OWNER_SPECIAL_MEMBERS(StateEdgeRequest);
    virtual void handleRequest(StateEdgeRequest*, Trigger) = 0;
    virtual void handleError(StateEdgeRequest*, const std::string&) = 0;
  };
  
 public:
  virtual ~StateEdgeRequest() = default;
  StateEdgeRequest(StateEdgeRequest&&) = delete;

  SET_OWNER();

  virtual void start() = 0; // this should only be called once
  
  // This class will trigger by default to a high and low state.
  // For TriggerSetting::None, the duration is not used
  void setTrigger(TriggerSetting,
		  const std::chrono::milliseconds& dur);

 protected:
 StateEdgeRequest(Owner* o) // ensure it is only instantiated by a child class
   : d_owner(o)
    {
      assert(o != nullptr);
    }

  void processState(bool); // call this with the new state
  void ownerError(const std::string&);
  
 protected:
  CORE::Owner<Owner> d_owner;
  KERN::AsioCallbackTimer d_error_timer = KERN::AsioCallbackTimer("StateEdgeRequest - Error timer.");
  std::string d_err_msg;
  
 private:
  bool d_on_off_wait_set = false;
  TriggerSetting d_trigger_set = TriggerSetting::None;
  KERN::AsioCallbackTimer d_state_timer = KERN::AsioCallbackTimer("StateEdgeRequest - State timer.");
};

#endif
