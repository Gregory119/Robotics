#include "dgp_controller.h"

#include "dgp_factory.h"

#include <cassert>
#include <iostream>

using namespace D_GP;

//----------------------------------------------------------------------//
GoProController::GoProController(GoProControllerOwner* o, ControlType type)
{
  assert(o!=nullptr);
  d_owner = o;
  d_gp.reset(GoProFactory::createGoPro(type, this));
}

//----------------------------------------------------------------------//
void GoProController::connectWithName(const std::string& name)
{
  d_gp_cmd_list.push_back(Request::Connect);
  d_gp->connectWithName(name);
}

//----------------------------------------------------------------------//
void GoProController::takePicture()
{
  d_gp_cmd_list.push_back(Request::Picture);
  if (!d_gp->isConnected())
    {
      d_owner->handleFailedRequest(this, Request::Picture);
      return;
    }
  d_gp->setMode(D_GP::Mode::Photo);
}

//----------------------------------------------------------------------//
void GoProController::takeMultiShot()
{
  d_gp_cmd_list.push_back(Request::MultiShot);
  if (!d_gp->isConnected())
    {
      d_owner->handleFailedRequest(this, Request::MultiShot);
      return;
    }
  d_gp->setMode(D_GP::Mode::MultiShot);
}

//----------------------------------------------------------------------//
void GoProController::StartStopRecording()
{
  d_gp_cmd_list.push_back(Request::StartStopRec);
  if (!d_gp->isConnected())
    {
      d_owner->handleFailedRequest(this, Request::StartStopRec);
      return;
    }
  d_gp->setMode(D_GP::Mode::Video);
}

//----------------------------------------------------------------------//
void GoProController::handleModeChanged(GoPro*, Mode mode)
{
  switch (mode)
    {
    case Mode::Photo:
    case Mode::MultiShot:
      d_gp->setShutter(true);
      break;

    case Mode::Video:
      if (d_is_recording)
	{
	  d_gp->setShutter(false);
	  d_is_recording = false;
	}
      else
	{
	  d_gp->setShutter(true);
	  d_is_recording = true;
	}
      break;
    };
}

//----------------------------------------------------------------------//
void GoProController::handleShutterSet(GoPro*, bool state)
{
  assert(!d_gp_cmd_list.empty());
  if (d_gp_cmd_list.front() == Request::StartStopRec)
    {
      d_is_recording = !d_is_recording;
    }
  d_gp_cmd_list.erase(d_gp_cmd_list.begin());
}

//----------------------------------------------------------------------//
void GoProController::handleFailedCommand(GoPro*, Cmd cmd)
{
  std::cout << "handleFailedCommand: " << cmdToString(cmd) << std::endl;
  assert(!d_gp_cmd_list.empty());
  
  d_owner->handleFailedRequest(this, d_gp_cmd_list.front());
  d_gp_cmd_list.erase(d_gp_cmd_list.begin());
}
