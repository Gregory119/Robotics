#ifndef KN_STDKERNEL_H
#define KN_STDKERNEL_H

#include <list>

namespace KERN
{
  class CallbackTimer;
  class StdKernel 
  {
    // A service that currently only processes timer timeouts
  public:
    StdKernel();
    StdKernel& operator=(const StdKernel&) = delete;
    StdKernel(const StdKernel&) = delete;
    virtual ~StdKernel() = default;
    
    static void registerTimer(CallbackTimer&);
    static void removeTimer(CallbackTimer&);
    void run(); //updates timer values, checks timeouts, and calls KernelTimer owner handler if necessary

  private:
    void process();
    
  private:
    std::list<CallbackTimer*> d_timers;
  };
};

#endif
