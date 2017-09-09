#ifndef KN_TIMER_H
#define KN_TIMER_H

#include "kn_callbacktimer.h"

namespace KERN
{
  class StdKernel;
  class KernelTimer;
  class KernelTimerOwner
  {
    //Inherited by class who wants to use kernel timers (Class 1).
    //Class 1 must have member timers, which it must own.
  protected:
    KernelTimerOwner() = default;
    ~KernelTimerOwner() = default;
    
  private:
    friend KernelTimer;
    virtual bool handleTimeOut(const KernelTimer&) = 0; //return true if timer found. Use function 'is(..)' to compare timers.
  };

  class KernelTimer final
  {    
  public:
    KernelTimer(KernelTimerOwner*);
    KernelTimer(const KernelTimer&);
    KernelTimer& operator=(const KernelTimer&) = delete;

    bool is(const KernelTimer& timer) const { return &timer == this; }
    bool is(const std::unique_ptr<KernelTimer>& timer) const { return timer.get() == this; }
    
    void restartMs(long time_ms);
    void restartMsIfNotSet(long time_ms);
    void restart(); //Re-enable with the timeout count from zero. Timeout time needs to have already been set.
    void disable();
    //Any restarts or disabling will set the consequetive time out count to zero.
    long getConseqTimeOuts() { return d_callback_timer->getConseqTimeOuts(); }
    long getTotalTimeOuts() { return d_callback_timer->getTotalTimeOuts(); }
    
    bool isSet() const { return d_callback_timer->isSet(); }
    bool isDisabled() { return d_callback_timer->isDisabled(); }
    void setTimeMs(long);
    bool hasTimedOut();

  private:
    friend StdKernel;

  private:
    KernelTimerOwner* d_owner = nullptr;
    std::unique_ptr<CallbackTimer> d_callback_timer;
  };
};

#endif
