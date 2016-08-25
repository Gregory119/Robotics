#ifndef KN_BASIC
#define KN_BASIC

namespace KERN
{
  class KernBasicComponent;

  class KernBasic
  {
  public:
  KernBasic(KernBasicComponent* comp)
      //does not take ownership
    : d_comp(comp)
    {}
    
    void run();

  private:
    KernBasicComponent *d_comp = nullptr;
  };

  class KernBasicComponent
  {
  private:
    friend KernBasic;
    virtual bool stayRunning() = 0;
  };
};

#endif
