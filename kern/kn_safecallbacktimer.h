#ifndef KN_SAFECALLBACKTIMER_H
#define KN_SAFECALLBACKTIMER_H

#include "kn_asiocallbacktimer.h"

#include <mutex>

namespace KERN
{
  // A thread safe asio callback timer.
  
  // The boost documentation describes the deadline timer as being thread safe for each separate (distinct) instance,
  // but not for shared instances between threads. The asio library guarantees that callback handlers will only be
  // called from threads that are currently calling io_service::run(). In a multithreaded program, only one asio kernel,
  // which calls io_service::run(), should be used to ensure timer callbacks are not called concurrently. io_service
  // is thread safe for shared instances except for the restart() and notify_fork() funcitons.
  class SafeCallbackTimer final : AsioCallbackTimer
  {
  public:
    SafeCallbackTimer() = default;
    SafeCallbackTimer(std::string name);
    SafeCallbackTimer(SafeCallbackTimer&&); // implement this so that a running timer will continue to run
    SafeCallbackTimer& operator=(SafeCallbackTimer&&);
    // implicitly not copyable from unique pointer, and cannot move the deadline timer
 
    void setTimeoutCallback(std::function<void()> callback) override; // this must be called before starting timer unless stated otherwise
    void restart(const std::chrono::milliseconds&) override;
    
    // The time must be set for the timer to restart
    void restart() override; // re-enable with the already set time

    void restartIfNotSet(const std::chrono::milliseconds&) override;
    void restartIfNotSetOrDisabled(const std::chrono::milliseconds&) override;
    
    void singleShot(const std::chrono::milliseconds&) override;
    void singleShotZero() override;
    void singleShotZero(std::function<void()> callback) override;
    void disable() override;
    void disableIfEnabled() override;

    bool isDisabled() override;
    
    // Any restarts or disabling will set the consequetive time out count to zero.
    // Will eventually overflow if timer repeats timeouts and is never disabled.
    long getConseqTimeOuts() override;

    void setTime(const std::chrono::milliseconds&) override;

  private:
    void timerCallBack(const boost::system::error_code& err,
		       boost::asio::deadline_timer* t) override;
    
  private:
    std::mutex d_m;
  };
};

#endif
