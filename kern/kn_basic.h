#ifndef KN_BASIC
#define KN_BASIC

namespace KERN
{
  class KernBasicComponent;

  class KernBasic //make a singleton
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
  public:
    ~KernBasicComponent() = default;
    
  private:
    friend KernBasic;
    virtual bool process() = 0;
  };
};

#endif
