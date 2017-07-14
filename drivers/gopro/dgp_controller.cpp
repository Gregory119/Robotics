#include "dgp_controller.h"

#include "dgp_factory.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

//----------------------------------------------------------------------//
GoProController::GoProController(GoProControllerOwner* o, GPCtrlParams p)
  : d_owner(o),
		d_gp(GoProFactory::createGoPro(p.ctrl_type, this)),
    d_connect_name(p.connect_name),
		d_state(new StateDisconnected),
		d_prev_state(new StateDisconnected)
{
  assert(o!=nullptr);
  
	// add list of possible states
  d_states[StateId::Connect] = std::unique_ptr<GPState>(new StateConnect());
  d_states[StateId::Photo] = std::unique_ptr<GPState>(new StatePhoto());
  d_states[StateId::StartStopRec] = std::unique_ptr<GPState>(new StateVideo());
  d_state = d_states[StateId::Connect].get();
	d_prev_state = d_state;
}

//----------------------------------------------------------------------//
void GoProController::takePhoto()
{
  d_state_id = StateId::Photo;
  setState(d_state_id);
}

//----------------------------------------------------------------------//
void GoProController::StartStopRecording()
{
  d_state_id = StateId::StartStopRec;
  setState(d_state_id);
}

//----------------------------------------------------------------------//
void GoProController::handleFailedCommand(GoPro*, Cmd cmd)
{
  std::cout << "handleFailedCommand: " << cmdToString(cmd) << std::endl;

	d_is_recording = false;
	setState(GPStateId::Disconnected);
	d_owner->handleFailedRequest(this);
}

//----------------------------------------------------------------------//
void GoProController::setState(StateId id)
{	
	d_prev_state_id = d_state_id;
	d_state_id = id;
	d_state = d_states[id].get();
	d_state->process();
}

//----------------------------------------------------------------------//
void StateConnect::process(GoProController& ctrl)
{
	assert(!d_connect_name.empty());
	assert(d_gp != nullptr);
	
	switch (ctrl.getPrevState())
		{
		case StateId::Connect:
			// do nothing
			break;
			
		case StateId::Photo:
			d_gp->connectWithName(d_connect_name);
			break;
			
		case StateId::StartStopRec:
			d_gp->connectWithName(d_connect_name);
			if (d_is_recording)
				{
					d_gp->setShutter(true); // stop recording
					d_is_recording = false;
				}
			break;

		default:
			assert(false);
			return;
			break;
		};
}

//----------------------------------------------------------------------//
void StatePhoto::process(GoProController& ctrl)
{
	assert(d_gp != nullptr);
	
	switch (ctrl.getPrevState())
		{
		case StateId::Connect:
			assert(!d_connect_name.empty());
			d_gp->connectWithName(d_connect_name);
			d_gp->setMode(Mode::Photo);
			d_gp->setShutter(true); // take pic
			break;
			
		case StateId::Photo:
			d_gp->setShutter(true); // take pic
			break;
			
		case StateId::StartStopRec:
			if (d_is_recording)
				{
					d_gp->setShutter(true); // stop recording
					d_is_recording = false;
				}
			d_gp->setMode(Mode::Photo);
			d_gp->setShutter(true); // take pic
			break;

		default:
			assert(false);
			return;
			break;
		};
}

//----------------------------------------------------------------------//
void StateVideo::process(GoProController& ctrl)
{
	assert(d_gp != nullptr);
	
	switch (ctrl.getPrevState())
		{
		case StateId::Connect:
			assert(!d_connect_name.empty());
			assert(!d_is_recording);
			d_gp->connectWithName(d_connect_name);
			d_gp->setMode(Mode::Video);
			d_gp->setShutter(true); // start video
			break;
			
		case StateId::Photo:
			assert(!d_is_recording);
			d_gp->setMode(Mode::Video);
			d_gp->setShutter(true); // start video
			break;
			
		case StateId::StartStopRec:
			d_is_recording = !d_is_recording;
			d_gp->setShutter(true); // start/stop video
			break;

		default:
			assert(false);
			return;
			break;
		};
}
