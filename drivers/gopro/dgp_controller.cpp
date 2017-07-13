#include "dgp_controller.h"

#include "dgp_factory.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

//----------------------------------------------------------------------//
GoProController::GoProController(GoProControllerOwner* o, GPCtrlParams p)
  : d_gp(GoProFactory::createGoPro(p.ctrl_type, this)),
    d_connect_name(p.connect_name)
{
  assert(o!=nullptr);
  d_owner = o;

  processState(); // process intially to connect
}

//----------------------------------------------------------------------//
void GoProController::connectWithName(const std::string& name)
{
  d_req = D_GP::Request::Connect;
  setState(D_GP::Mode:Connected);
}

//----------------------------------------------------------------------//
void GoProController::takePicture()
{
  d_req = D_GP::Request::Picture;
  setState(D_GP::Mode:Photo);
}

//----------------------------------------------------------------------//
void GoProController::takeMultiShot()
{
  d_req = D_GP::Request::MultiShot;
  setState(D_GP::Mode:MultiShot);
}

//----------------------------------------------------------------------//
void GoProController::StartStopRecording()
{
  d_req = D_GP::Request::StartStopRec;
  setState(D_GP::Mode:Video);
}

//----------------------------------------------------------------------//
void GoProController::handleModeSet(GoPro*, Mode mode)
{
  processState();
}

//----------------------------------------------------------------------//
void GoProController::handleShutterSet(GoPro*, bool state)
{
}

//----------------------------------------------------------------------//
void GoProController::handleFailedCommand(GoPro*, Cmd cmd)
{
  std::cout << "handleFailedCommand: " << cmdToString(cmd) << std::endl;
  
  d_owner->handleFailedRequest(this, d_req);
}

//----------------------------------------------------------------------//
void GoProController::setState(D_GP::Mode state)
{
  switch (d_prev_state)
    {
    case D_GP::Mode::Disconnected:
      assert(state == D_GP::Mode::Connected);
      break;

    case D_GP::Mode::Connected:
      assert(state != D_GP::Mode::Disconnected); // should not happen because no mode changed is requested in the connected state/mode
      break;
      
    case D_GP::Mode::MultiShot:
      if (!d_multishot_complete)
	{
	  //log
	  return; // do not change state until multishot has completed
	}
    case D_GP::Mode::Photo:
      if (d_is_recording)
	{
	  return;
	}
      break;
      
    case D_GP::Mode::Video:
      break;

    default:
      assert(false);
      return;
      break;
    }

  d_prev_state = d_state;
  d_state = state;
  d_gp->setMode(D_GP::Mode::Photo);
}

//----------------------------------------------------------------------//
void GoProController::processState()
{
  switch (d_state)
    {
    case D_GP::Mode::Disconnected:
      connectWithName(d_connect_name);
      setState(D_GP::Mode::Connected);
      break;

    case D_GP::Mode::Connected:
      assert(d_gp->isConnected());
      break;
      
    case D_GP::Mode::Photo:
      d_gp->setShutter(true);
      break;
      
    case D_GP::Mode::MultiShot:
      d_multishot_complete = false;
      d_gp->setShutter(true);
      //?????????start multishot wait timer for completion
      break;
      
    case D_GP::Mode::Video:
      d_is_recording = !d_is_recording;
      d_gp->setShutter(true);
      break;
      
    default:
      assert(false);
      return;
      break;
    }
}
