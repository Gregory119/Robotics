#ifndef CORE_TIMER_H
#define CORE_TIMER_H

namespace CORE
{
  class Timer;

  class TimerOwner : KERN::KernBasicComponent
  {
  public:
    KernBasicComponent(KernBasic*); 
    KernBasicComponent(KernBasicComponent* superior_comp);
    virtual ~TimerOwner() = default;
    
  private:
    friend Timer;
    virtual bool handleTimeOut(Timer*) = 0;
  };

  class Timer : KERN::KernBasicComponent
  {
  public:
    Timer(TimerOwner* o)
      : KERN::KernBasicComponent(o),
      d_owner(o)
      {}
    
    //create some timing class instance that is owned
    //can use boost cpu timers
    //startIfNotSet()
    //stop()
    //restart()
    //kill
    //handleTimeOut callback called when time increment passed

  private:
    std::unique_ptr<TimerOwner> d_owner;
  };
};

#endif
