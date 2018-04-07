#include "stateedgerequest.h"

//----------------------------------------------------------------------//
void StateEdgeRequest::setTrigger(TriggerSetting trigger_set,
				  const std::chrono::milliseconds& dur)
{
  d_trigger_set = trigger_set;
  d_state_timer.disableIfEnabled();
  
  switch (d_trigger_set)
    {
    case TriggerSetting::None:
      return;
      
    case TriggerSetting::Combination:
      d_state_timer.setTime(dur);
      d_state_timer.setTimeoutCallback([this](){
	  d_owner.call(&StateEdgeRequest::Owner::handleRequest,
		       this,
		       Trigger::AfterDuration);
	});
      return;
    }
  assert(false);
  std::ostringstream stream("setTrigger - invalid TriggerSetting of ",
			    std::ios_base::app);
  stream << static_cast<int>(d_trigger_set) << ".";
  ownerError(stream.str());
}

//----------------------------------------------------------------------//
void StateEdgeRequest::processState(bool state)
{
  switch (d_trigger_set)
    {
    case TriggerSetting::None:
      d_owner.call(&StateEdgeRequest::Owner::handleRequest,
		   this,
		   Trigger::None);
      return;
      
    case TriggerSetting::Combination:
      if (!d_on_off_wait_set)
	{
	  d_on_off_wait_set = true;
	  d_state_timer.singleShot();
	  return;
	}

      if (d_state_timer.isScheduledToExpire())
	{
	  d_on_off_wait_set = false;
	  d_state_timer.disable();
	  d_owner.call(&StateEdgeRequest::Owner::handleRequest,
		       this,
		       Trigger::OnOffBeforeDuration);
	  return;
	}
      d_state_timer.singleShot(); // on off wait still set
      return;
    }
  assert(false);
  std::ostringstream stream("processState - invalid TriggerSetting of ",
			    std::ios_base::app);
  stream << static_cast<int>(d_trigger_set) << ".";
  ownerError(stream.str());
}

//----------------------------------------------------------------------//
void StateEdgeRequest::ownerError(const std::string& msg)
{
  std::string new_msg = "StateEdgeRequest::";
  new_msg += msg;
  d_owner.call(&Owner::handleError, this, new_msg);
}
