#ifndef DGP_FACTORY_H
#define DGP_FACTORY_H

#include "dgp_gopro.h"
#include <string>

namespace D_GP
{
  class SimpleGoPro;
  class GoProFactory
  {
  public:
    static GoPro* createGoPro(GoPro::Owner*, CamModel, const std::string& name);
  };
};

#endif
