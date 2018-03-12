#include "kn_safecallbacktimer.h"

using namespace KERN;

//----------------------------------------------------------------------//
SafeCallbackTimer::SafeCallbackTimer(std::string name)
  : AsioCallbackTimer(std::move(name))
{}

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
  AsioCallbackTimer::operator=(std::move(rhs));
  return *this;
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::setTimeoutCallback(std::function<void()> callback)
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::setTimeoutCallback(callback);
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::restart(const std::chrono::milliseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::restart(time);
}
    
//----------------------------------------------------------------------//
void SafeCallbackTimer::restart()
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::restart();
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::restartIfNotSet(const std::chrono::milliseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::restartIfNotSet(time);
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::restartIfNotSetOrDisabled(const std::chrono::milliseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::restartIfNotSetOrDisabled(time);
}
    
//----------------------------------------------------------------------//
void SafeCallbackTimer::singleShot(const std::chrono::milliseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::singleShot(time);
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::singleShotZero()
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::singleShotZero();
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::singleShotZero(std::function<void()> callback)
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::singleShotZero(callback);
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::disable()
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::disable();
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::disableIfEnabled()
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::disableIfEnabled();
}

//----------------------------------------------------------------------//
bool SafeCallbackTimer::isDisabled()
{
  std::lock_guard<std::mutex> lk(d_m);
  return AsioCallbackTimer::isDisabled();
}
    
//----------------------------------------------------------------------//
long SafeCallbackTimer::getConseqTimeOuts()
{
  std::lock_guard<std::mutex> lk(d_m);
  return AsioCallbackTimer::getConseqTimeOuts();
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::setTime(const std::chrono::milliseconds& time)
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::setTime(time);
}

//----------------------------------------------------------------------//
void SafeCallbackTimer::timerCallBack(const boost::system::error_code& err,
				      boost::asio::deadline_timer* t)
{
  std::lock_guard<std::mutex> lk(d_m);
  AsioCallbackTimer::timerCallBack(err,t);
}
