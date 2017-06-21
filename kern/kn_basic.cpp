#include "kn_basic.h"

#include <cassert.h>

using namespace KERN;

//----------------------------------------------------------------------//
void KernBasic::run()
{
  assert(d_comp != nullptr);

  while (true)
    {
      if (!d_comp->process() || !d_comp->processSubComponents())
	{
	  assert(false);
	  return;
	}
    }
}

//----------------------------------------------------------------------//
bool KernBasicComponent::processSubComponents()
{
  bool ret = true;
  for (const auto& comp : d_sub_components)
    {
      assert(comp != nullptr);
      if (!d_comp->process() || !d_comp->processSubComponents())
	{
	  assert(false);
	  ret = false;
	  break;
	}
    }
  
  return ret;
}

//----------------------------------------------------------------------//
KernBasicComponent::KernBasicComponent(KernBasic* kern)
{
  assert(kern != nullptr);
}

//----------------------------------------------------------------------//
KernBasicComponent::KernBasicComponent(KernBasicComponent* superior_comp)
{
  superior_comp->registerSubKernComponent(this);
}

//----------------------------------------------------------------------//
void KernBasicComponent::registerSubKernComponent(KernBasicComponent* comp)
{
  assert(comp != nullptr);
  d_sub_components.push_back(comp);
}
