#ifndef KN_ASIOCALLBACKTIMER_H
#define KN_ASIOCALLBACKTIMER_H

#include <boost/asio.hpp>
#include <chrono>
#include <string>

namespace KERN
{
  class AsioKernel;
  class AsioCallbackTimer final
  {    
  public:
    AsioCallbackTimer();
    AsioCallbackTimer(std::string name);
    AsioCallbackTimer(AsioCallbackTimer&&) = default; // implement this so that a running timer will continue to run
    AsioCallbackTimer& operator=(AsioCallbackTimer&&) = default;
    // implicitly not copyable from unique pointer, and cannot move the deadline timer
 
    void setTimeoutCallback(std::function<void()> callback); // this must be called before starting timer unless stated otherwise
    void restartMs(long time_ms);
    void restart(std::chrono::milliseconds);
    void restartMsIfNotSet(long time_ms);
    void restartMsIfNotSetOrDisabled(long time_ms);
    void restart(); // re-enable with the already set time
    void singleShot(std::chrono::milliseconds);
    void disable();
    
    // Any restarts or disabling will set the consequetive time out count to zero.
    // Will eventually overflow if timer repeats timeouts and is never disabled.
    long getConseqTimeOuts() { return d_count_conseq_timeouts; }
    
    bool isSet() const { return d_timeout_ms>=0; }
    bool isDisabled() { return !d_is_enabled; }
    void setTimeMs(long);
    
  private:
    void timerCallBack(const boost::system::error_code& err,
		       boost::asio::deadline_timer* t);

  private:
    const std::string d_name;
    
    std::function<void()> d_timeout_callback;
    long d_timeout_ms = -1;
    bool d_is_enabled = false;
    long d_count_conseq_timeouts = 0;
    bool d_is_single_shot = false;

    std::unique_ptr<boost::asio::deadline_timer> d_timer;
  };
};

#endif
