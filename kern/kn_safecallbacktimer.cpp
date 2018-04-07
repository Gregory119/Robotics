#include "kn_safecallbacktimer.h"

using namespace KERN;

//----------------------------------------------------------------------//
SafeCallbackTimer::SafeCallbackTimer()
  : d_timer(new AsioCallbackTimer())
{
  d_timer->setInternalTimerCallback([this](const boost::system::error_code& err){
      std::lock_guard<std::mutex> lk(d_m);
      d_timer->timerCallback(err);
    });
}

//----------------------------------------------------------------------//
SafeCallbackTimer::SafeCallbackTimer(std::string name)
  : d_timer(new AsioCallbackTimer(std::move(name)))
{
  d_timer->setInternalTimerCallback([this](const boost::system::error_code& err){
      std::lock_guard<std::mutex> lk(d_m);
      d_timer->timerCallback(err);
    });
}

//----------------------------------------------------------------------//
SafeCallbackTimer::SafeCallbackTimer(SafeCallbackTimer&& rhs)
{
  *this = std::move(rhs);
}

//----------------------------------------------------------------------//
SafeCallbackTimer& SafeCallbackTimer::operator=(SafeCallbackTimer&& rhs)
{
  std::lock_guard<std::mutex> lk_rhs(rhs.d_m);
  std::lock_guard<std::mutex> lk(d_m);
  d_timer = std::move(rhs.d_timer);
  d_timer->setInternalTimerCallback([this](const boost::system::error_code& err){
      std::lock_guard<std::mutex> lk(d_m);
      d_timer->timerCallback(err);
    });
  return *this;
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::setTimeoutCallback(std::function<void()> callback)
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->setTimeoutCallback(callback);
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::restart(const std::chrono::microseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->restart(time);
}
    
//----------------------------------------------------------------------//
void SafeCallbackTimer::restart()
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->restart();
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::restartIfNotSet(const std::chrono::microseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->restartIfNotSet(time);
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::restartIfNotSetOrDisabled(const std::chrono::microseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->restartIfNotSetOrDisabled(time);
}
    
//----------------------------------------------------------------------//
void SafeCallbackTimer::singleShot(const std::chrono::microseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->singleShot(time);
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::singleShotZero()
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->singleShotZero();
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::singleShotZero(std::function<void()> callback)
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->singleShotZero(callback);
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::disable()
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->disable();
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::disableIfEnabled()
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->disableIfEnabled();
}

//----------------------------------------------------------------------//
bool SafeCallbackTimer::isDisabled()
{
  std::lock_guard<std::mutex> lk(d_m);
  return d_timer->isDisabled();
}

//----------------------------------------------------------------------//
bool SafeCallbackTimer::isScheduledToExpire()
{
  std::lock_guard<std::mutex> lk(d_m);
  return d_timer->isScheduledToExpire();
}

//----------------------------------------------------------------------//
long SafeCallbackTimer::getConseqTimeOuts()
{
  std::lock_guard<std::mutex> lk(d_m);
  return d_timer->getConseqTimeOuts();
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::setTime(const std::chrono::microseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  d_timer->setTime(time);
}
