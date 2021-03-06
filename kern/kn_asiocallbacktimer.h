#ifndef KN_ASIOCALLBACKTIMER_H
#define KN_ASIOCALLBACKTIMER_H

#include <boost/asio.hpp>
#include <chrono>
#include <string>

namespace KERN
{
  class AsioKernel;
  class SafeCallbackTimer;
  class AsioCallbackTimer
  {    
  public:
    AsioCallbackTimer();
    AsioCallbackTimer(std::string name);
    virtual ~AsioCallbackTimer() = default;

    // declared move members implicitly delete copy members
    AsioCallbackTimer(AsioCallbackTimer&&) = default; // implement this so that a running timer will continue to run
    AsioCallbackTimer& operator=(AsioCallbackTimer&&) = default;
    // implicitly not copyable from unique pointer, and cannot move the deadline timer
 
    void setTimeoutCallback(std::function<void()> callback); // this must be called before starting timer unless stated otherwise
    void restart(const std::chrono::microseconds&);
    
    // The time must be set for the timer to restart
    void restart(); // re-enable with the already set time

    void restartIfNotSet(const std::chrono::microseconds&);
    void restartIfNotSetOrDisabled(const std::chrono::microseconds&);

    void singleShot(const std::chrono::microseconds&);
    void singleShotZero();
    void singleShot(); // time has already been set
    void singleShotZero(std::function<void()> callback);
    void disable();
    void disableIfEnabled();

    bool isDisabled() { return !d_is_enabled; }
    bool isScheduledToExpire() { return d_is_enabled; }
    
    // Any restarts or disabling will set the consequetive time out count to zero.
    // Will eventually overflow if timer repeats timeouts and is never disabled.
    long getConseqTimeOuts();

    void setTime(const std::chrono::microseconds&);

  protected:
    friend class SafeCallbackTimer;
    void setInternalTimerCallback(std::function<void(const boost::system::error_code& err)>);
    void timerCallback(const boost::system::error_code& err);

  private:
    void scheduleCallback();

  private:
    std::string d_name;
    
    std::function<void()> d_timeout_callback;
    std::function<void(const boost::system::error_code& err)> d_internal_callback;
    
    std::chrono::microseconds d_timeout = std::chrono::microseconds(0);
    bool d_is_set = false;
    
    bool d_is_enabled = false;
    long d_count_conseq_timeouts = 0;
    bool d_is_single_shot = false;

    std::unique_ptr<boost::asio::deadline_timer> d_timer;
  };
};

#endif
