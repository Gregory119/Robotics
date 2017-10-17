#include "dgp_factory.h"

#include "dgp_hero5.h"

#include <cassert>
#include <memory>

using namespace D_GP;

//----------------------------------------------------------------------//
GoPro* GoProFactory::createGoPro(GoPro::Owner* o,
				 CamModel model,
				 const std::string& name)
{
  switch (model)
    {
    case CamModel::Hero5:
      return new GoProHero5(o, name);

    case CamModel::Unknown:
      assert(false);
      return nullptr;
    };

  // LOG the model type here since it was not picked up
  assert(false);
  return nullptr;
}
