#ifndef KN_TIMER_H
#define KN_TIMER_H

#include "core_timer.h"

namespace KERN
{
  class StdKernel;
  class KernelTimer;
  class KernelTimerOwner
  {
    //Inherited by class who wants to use kernel timers (Class 1).
    //Class 1 must have member timers, which it must own.
  public:
    virtual ~KernelTimerOwner() = default;

  private:
    friend KernelTimer;
    virtual bool handleTimeOut(const KernelTimer&) = 0; //return true if timer found. Use == to compare timers.
  };

  class KernelTimer : CORE::Timer
  {    
  public:
    KernelTimer(KernelTimerOwner*);
    virtual ~KernelTimer();
    KernelTimer(const KernelTimer&) = delete; //currently not copyable (consider owner pointer)
    KernelTimer& operator=(const KernelTimer&) = delete;

    bool operator==(const KernelTimer& rhs) const { return &rhs == this; }
    bool operator==(const KernelTimer* rhs) const { return operator==(*rhs); }
    bool operator!=(const KernelTimer& rhs) const { return !operator==(rhs); }
    bool operator!=(const KernelTimer* rhs) const { return operator!=(*rhs); }
    
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

  private:
    friend StdKernel;
    bool processTimeOut(); //update time and process timeout if it has occured
    bool hasTimedOut();

  private:
    KernelTimerOwner* d_owner = nullptr;
    long d_timeout_ms = -1;
    bool d_is_enabled = false;
    long d_count_conseq_timeouts = 0;
    long d_total_timeouts = 0;
  };
};

#endif
