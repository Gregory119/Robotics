#include "stateedgerequest.h"

//----------------------------------------------------------------------//
void StateEdgeRequest::activateDurationTriggers(const std::chrono::milliseconds& dur,
						bool trig_state)
{
  d_has_dur_trig = true;
  d_state_timer.setTime(dur);

  if (!d_has_dur_trig)
    {
      d_state_timer.disableIfEnabled();
      return;
    }
  
  d_trig_state = trig_state;
  d_state_timer.disableIfEnabled();
  d_state_timer.setTimeoutCallback([this](){
      d_owner.call(StateEdgeRequest::Owner::handleRequest,
		   this,
		   DurationTrigger::AfterDuration);
    });
}

//----------------------------------------------------------------------//
void StateEdgeRequest::processState(bool state)
{
  if (!d_has_dur_trig)
    {
      d_owner.call(StateEdgeRequest::Owner::handleRequest,
		   this,
		   DurationTrigger::None);
      return;
    }
  
  if (state == d_trig_state)
    {
      d_on_off_wait_set = true;
      d_state_timer.singleShot();
      return;
    }

  // state != d_trig_state
  // if changed before timeout then trigger
  if (d_on_off_wait_set && d_state_timer.isScheduledToExpire())
    {
      d_on_off_wait_set = false;
      d_state_timer.disable();
      d_owner.call(StateEdgeRequest::Owner::handleRequest,
		   this,
		   DurationTrigger::OnOffBeforeDuration);
      return;
    }
  return;
}
