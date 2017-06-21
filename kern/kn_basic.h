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
  public:
    KernBasicComponent(KernBasic*);
    KernBasicComponent(KernBasicComponent* superior_comp); // needs a superior component to call it

  private:
    registerSubKernComponent(KernBasicComponent*);
    
  private:
    friend KernBasic;
    virtual bool process() { return true; }
    bool processSubComponents();

    std::list<KernBasicComponent> d_sub_components;
  };
};

#endif
