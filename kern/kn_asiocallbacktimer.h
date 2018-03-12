#ifndef KN_ASIOCALLBACKTIMER_H
#define KN_ASIOCALLBACKTIMER_H

#include <boost/asio.hpp>
#include <chrono>
#include <string>

namespace KERN
{
  class AsioKernel;
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
 
    virtual void setTimeoutCallback(std::function<void()> callback); // this must be called before starting timer unless stated otherwise
    virtual void restart(const std::chrono::milliseconds&);
    
    // The time must be set for the timer to restart
    virtual void restart(); // re-enable with the already set time

    virtual void restartIfNotSet(const std::chrono::milliseconds&);
    virtual void restartIfNotSetOrDisabled(const std::chrono::milliseconds&);
    
    virtual void singleShot(const std::chrono::milliseconds&);
    virtual void singleShotZero();
    virtual void singleShotZero(std::function<void()> callback);
    virtual void disable();
    virtual void disableIfEnabled();

    virtual bool isDisabled();
    
    // Any restarts or disabling will set the consequetive time out count to zero.
    // Will eventually overflow if timer repeats timeouts and is never disabled.
    virtual long getConseqTimeOuts();

    virtual void setTime(const std::chrono::milliseconds&);

  protected:
    virtual void timerCallBack(const boost::system::error_code& err,
			       boost::asio::deadline_timer* t);

  private:
    void scheduleCallback();

  private:
    std::string d_name;
    
    std::function<void()> d_timeout_callback;
    
    std::chrono::milliseconds d_timeout = std::chrono::milliseconds(0);
    bool d_is_set = false;
    
    bool d_is_enabled = false;
    long d_count_conseq_timeouts = 0;
    bool d_is_single_shot = false;

    std::unique_ptr<boost::asio::deadline_timer> d_timer;
  };
};

#endif
