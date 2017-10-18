#ifndef KN_ASIOCALLBACKTIMER_H
#define KN_ASIOCALLBACKTIMER_H

#include <boost/asio.hpp>

namespace KERN
{
  class AsioKernel;
  class AsioCallbackTimer final
  {    
  public:
    AsioCallbackTimer();
    AsioCallbackTimer(const AsioCallbackTimer&) = delete;
    AsioCallbackTimer& operator=(const AsioCallbackTimer&) = delete;
    AsioCallbackTimer& operator=(AsioCallbackTimer&&) = delete;
    AsioCallbackTimer(AsioCallbackTimer&&) = delete;

    void setCallback(std::function<void()> callback); // this must be called before starting timer
    void restartMs(long time_ms);
    void restartMsIfNotSet(long time_ms);
    void restartMsIfNotSetElseDisabled(long time_ms);
    void restart(); // re-enable with the already set time
    void disable();
    //Any restarts or disabling will set the consequetive time out count to zero.
    long getConseqTimeOuts() { return d_count_conseq_timeouts; }
    
    bool isSet() const { return d_timeout_ms>=0; }
    bool isDisabled() { return !d_is_enabled; }
    void setTimeMs(long);

  private:
    void timerCallBack(const boost::system::error_code& err,
		       boost::asio::deadline_timer* t);

  private:
    std::function<void()> d_user_callback;
    long d_timeout_ms = -1;
    bool d_is_enabled = false;
    long d_count_conseq_timeouts = 0;

    std::unique_ptr<boost::asio::deadline_timer> d_timer;
  };
};

#endif
