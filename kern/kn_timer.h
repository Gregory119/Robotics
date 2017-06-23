#ifndef KN_TIMER_H
#define KN_TIMER_H

namespace KERN
{
//timer kernel
//loop
//can be notified to process a handletimeout of timer
  class KernelTimer;
  using KernelTimer::Priority = unsigned char; //0 is most important
  class StdKernel 
  {
    //must be singleton to be called by timer
  public:
    StdKernel();
    StdKernel& operator=(const StdKernel&) = delete; //should never need to copy
    StdKernel(const StdKernel&) = delete;
    virtual ~StdKernel() = default;
    
    static registerTimer(const KernelTimer&); //called by KernelTimerOwner
    run(); //checks timeouts of all timers and calls KernelTimer owner handler

  private:
    std::multimap<KernelTimer::Priority, KernelTimer*> d_timers;
  };

  class KernelTimerOwner
  {
    //inherited by class who wants to use kernel timers
  public:
    virtual ~KernelTimerOwner() = default;

  private:
    friend KernelTimer;
    virtual bool handleTimeOut(const KernelTimer&) = 0; //return true if timer found. Use == to compare timers.
  };

  class KernelTimer : Timer
 {
 public:
   KernelTimer(KernelTimerOwner*);
   
   processTimeOut(); //check if timed out and call owners time out handle function
   restartMs(unsigned time_ms);
   restartMsIfNotSet(unsigned time_ms);
   kill();
   
   bool operator==(const KernelTimer&) const; //used to check pointers
   bool isSet() const { return d_timeout_ms>0; }
   void setTimeOutMs(int);
   void setPriority(Priority);
   bool hasTimedOut() const;

 private:
   KernelTimerOwner* d_owner = nullptr;
   Priority d_priority = 10;
   int d_timeout_ms = -1;
   bool d_is_timedout = false;

   //has a timer that works on a thread timing
 };

 //Timer
 //start
 //stop
 //reset
 //getTimeMs
};

#endif
