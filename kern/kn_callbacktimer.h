#ifndef KN_CALLBACKTIMER_H
#define KN_CALLBACKTIMER_H

#include "core_timer.h"

namespace KERN
{
  class StdKernel;
  class CallbackTimer final : public CORE::Timer
  {    
  public:
    CallbackTimer();
    ~CallbackTimer();
    CallbackTimer(const CallbackTimer&) = delete;
    CallbackTimer& operator=(const CallbackTimer&) = delete;
    // move operators??

    void setCallback(std::function<void()> callback); // this must be called before starting timer
    void restartMs(long time_ms);
    void restartMsIfNotSet(long time_ms);
    void restart(); //Re-enable with the timeout count from zero. Timeout time needs to have already been set.
    void disable();
    //Any restarts or disabling will set the consequetive time out count to zero.
    long getConseqTimeOuts() { return d_count_conseq_timeouts; }
    long getTotalTimeOuts() { return d_total_timeouts; }
    
    bool isSet() const { return d_timeout_ms>=0; }
    bool isDisabled() { return !d_is_enabled; }
    void setTimeMs(long);
    bool hasTimedOut();

  private:
    friend StdKernel;
    void processTimeOut(); //update time and process timeout if it has occured

  private:
    std::function<void()> d_callback;
    long d_timeout_ms = -1;
    bool d_is_enabled = false;
    long d_count_conseq_timeouts = 0;
    long d_total_timeouts = 0;
  };
};

#endif
