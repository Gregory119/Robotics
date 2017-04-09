#include "dgp_controller.h"

#include "dgp_factory.h"

#include <cassert>

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
  d_gp->connectWithName(name);
}

//----------------------------------------------------------------------//
void GoProController::takePicture()
{
  d_gp_cmd_list.emplace_back(Request::Picture);
  if (!d_gp->isConnected())
    {
      d_owner->handleFailedRequest(this, *d_gp_cmd_list.end());
      return;
    }
  d_gp->setMode(D_GP::Mode::Photo);
}

//----------------------------------------------------------------------//
void GoProController::takeMultiShot()
{
  d_gp_cmd_list.emplace_back(Request::MultiShot);
  if (!d_gp->isConnected())
    {
      d_owner->handleFailedRequest(this, *d_gp_cmd_list.end());
      return;
    }
  d_gp->setMode(D_GP::Mode::Photo);
}

//----------------------------------------------------------------------//
void GoProController::StartStopRecording()
{
  d_gp_cmd_list.emplace_back(Request::StartStopRec);
  if (!d_gp->isConnected())
    {
      d_owner->handleFailedRequest(this, *d_gp_cmd_list.end());
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
	}
      else
	{
	  d_gp->setShutter(true);
	}
      break;
    };
}

//----------------------------------------------------------------------//
void GoProController::handleShutterSet(GoPro*, bool state)
{
  if (*d_gp_cmd_list.begin() == Request::StartStopRec)
    {
      d_is_recording = !d_is_recording;
    }
  d_gp_cmd_list.pop_front();
}

//----------------------------------------------------------------------//
void GoProController::handleFailedCommand(GoPro*, Cmd)
{
  d_owner->handleFailedRequest(this, *d_gp_cmd_list.begin());
  d_gp_cmd_list.pop_front();
}
