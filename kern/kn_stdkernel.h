#ifndef KN_STDKERNEL_H
#define KN_STDKERNEL_H

#include <list>

namespace KERN
{
  class KernelTimer;
  class StdKernel 
  {
    // A service that currently only processes timer timeouts
  public:
    StdKernel();
    StdKernel& operator=(const StdKernel&) = delete;
    StdKernel(const StdKernel&) = delete;
    virtual ~StdKernel() = default;
    
    static void registerTimer(KernelTimer&); //called by KernelTimerOwner
    int run(); //updates timer values, checks timeouts, and calls KernelTimer owner handler if necessary

  private:
    int process();
    
  private:
    std::list<KernelTimer*> d_timers;
  };
};

#endif
