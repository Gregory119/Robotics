#include "dgp_factory.h"

#include "dgp_hero5.h"

#include <cassert>
#include <memory>

using namespace D_GP;

//----------------------------------------------------------------------//
GoPro* GoProFactory::createGoPro(CamModel model, GoProOwner* o)
{
  switch (type)
    {
    case CamModel::Hero5:
      return new GoProHero5(o);
    };

  // LOG the model type here since it was not picked up
  assert(false);
}
