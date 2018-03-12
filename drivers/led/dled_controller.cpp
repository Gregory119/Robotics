#include "dled_controller.h"

using namespace D_LED;

//----------------------------------------------------------------------//
Controller::Controller(Owner* o, const std::string& driver_dir)
  : d_owner(o),
    d_led_driver(new Driver(this, driver_dir))
{
  assert(o != nullptr);
}

//----------------------------------------------------------------------//
Controller::~Controller()
{
  useMemory();
}

//----------------------------------------------------------------------//
void Controller::setOwner(Owner* o)
{
  assert(o != nullptr);
  d_owner = o;
}

//----------------------------------------------------------------------//
void Controller::useMemory()
{
  startReq(Req::Memory);
}

//----------------------------------------------------------------------//
void Controller::turnOn()
{
  startReq(Req::On);
}

//----------------------------------------------------------------------//
void Controller::turnOff()
{
  startReq(Req::Off);
}

//----------------------------------------------------------------------//
void Controller::flashOnOff(const std::chrono::milliseconds& delay_on,
			    const std::chrono::milliseconds& delay_off)
{
  d_reqs_flash_on_off.push_back({delay_on, delay_off});
  startReq(Req::FlashOnOff);
}

//----------------------------------------------------------------------//
void Controller::flashPerSec(unsigned rate)
{
  d_reqs_flash_per_secs.push_back(rate);
  startReq(Req::FlashPerSec);
}

//----------------------------------------------------------------------//
void Controller::flashAdvanced(const Driver::AdvancedSettings& set)
{
  d_reqs_adv_settings.emplace_back(set);
  startReq(Req::FlashAdvanced);
}

//----------------------------------------------------------------------//
void Controller::handleFlashCycleEnd(Driver*)
{
  popFrontReq();
  if (d_owner != nullptr)
    {
      d_owner->handleFlashCycleEnd(this);
    }
  processReq();
}

//----------------------------------------------------------------------//
void Controller::handleError(Driver* driver,
			     Driver::Error e,
			     const std::string& msg)
{
  ownerReqFailed("Controller::handleReqFailed() " + msg);
}

//----------------------------------------------------------------------//
void Controller::startReq(Req req)
{
  bool is_processing_reqs = !d_reqs.empty();

  // Replace the last request on the list if the last request on the list
  // is a command that will not have time to be seen by the user.
  if (is_processing_reqs)
    {
      switch (d_reqs.back())
	{
	case Req::Memory:
	case Req::On:
	case Req::Off:
	case Req::FlashOnOff:
	case Req::FlashPerSec:
	  popBackReq();
	  break;
	  
	case Req::FlashAdvanced:
	  replaceReqFlashAdvanced();
	  break;
	}
    }

  d_reqs.emplace_back(req);
  if (!is_processing_reqs)
    {
      processReq();
    }
}

//----------------------------------------------------------------------//
void Controller::replaceReqFlashAdvanced()
{
  switch (d_reqs_adv_settings.back().cycle)
    {
    case Driver::FlashCycle::Continuous:
      popBackReq();
      return;
      
    case Driver::FlashCycle::OnceOff:
    case Driver::FlashCycle::RepeatLimit:
      return;
    }
  assert(false);
  ownerInternalError();
}

//----------------------------------------------------------------------//
void Controller::popFrontReq()
{
  d_reqs.pop_front();
}

//----------------------------------------------------------------------//
void Controller::popBackReq()
{
  assert(!d_reqs.empty());
  Req back = d_reqs.back();
  d_reqs.pop_back();
  
  switch (back)
    {
    case Req::Memory:
    case Req::On:
    case Req::Off:
      return;
      
    case Req::FlashOnOff:
      assert(!d_reqs_flash_on_off.empty());
      d_reqs_flash_on_off.pop_back();
      return;
      
    case Req::FlashPerSec:
      assert(!d_reqs_flash_per_secs.empty());
      d_reqs_flash_per_secs.pop_back();
      return;
      
    case Req::FlashAdvanced:
      assert(!d_reqs_adv_settings.empty());
      d_reqs_adv_settings.pop_back();
      return;
    }
  assert(false);
  ownerInternalError();
}

//----------------------------------------------------------------------//
void Controller::processReq()
{
  if (d_reqs.empty())
    {
      return;
    }
  
  switch (d_reqs.front())
    {
    case Req::Memory:
      d_led_driver->useMemory();
      popFrontReq();
      processReq();
      return;
      
    case Req::On:
      d_led_driver->turnOn();
      popFrontReq();
      processReq();
      return;
      
    case Req::Off:
      d_led_driver->turnOff();
      popFrontReq();
      processReq();
      return;
      
    case Req::FlashOnOff:
      d_led_driver->flashOnOff(d_reqs_flash_on_off.front().delay_on,
			       d_reqs_flash_on_off.front().delay_off);
      popFrontReq();
      processReq();
      return;
      
    case Req::FlashPerSec:
      d_led_driver->flashPerSec(d_reqs_flash_per_secs.front());
      popFrontReq();
      processReq();
      return;
      
    case Req::FlashAdvanced:
      internalFlashAdvanced();
      return;
    }
  assert(false);
  ownerInternalError();
}

//----------------------------------------------------------------------//
void Controller::internalFlashAdvanced()
{
  const Driver::AdvancedSettings& set = d_reqs_adv_settings.front();
  d_led_driver->flashAdvanced(set);

  switch (set.cycle)
    {
    case Driver::FlashCycle::Continuous:
      popFrontReq();
      processReq();
      return;
	  
    case Driver::FlashCycle::OnceOff:
    case Driver::FlashCycle::RepeatLimit:
      return;
    }
  assert(false);
  ownerInternalError();
}

//----------------------------------------------------------------------//
void Controller::ownerReqFailed(const std::string& err_msg)
{
  if (d_owner != nullptr)
    {
      d_owner->handleReqFailed(this, d_reqs.front(), err_msg);
    }
}

//----------------------------------------------------------------------//
void Controller::ownerInternalError()
{
  if (d_owner != nullptr)
    {
      d_owner->handleInternalError(this);
    }
}
