#ifndef DGP_FACTORY_H
#define DGP_FACTORY_H

#include "dgp_gopro.h"

namespace D_GP
{
  class SimpleGoPro;
  class GoProFactory
  {
  public:
    static GoPro* createGoPro(CamModel, GoProOwner*);
  };
};

#endif
