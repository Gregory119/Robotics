#include "dgp_factory.h"

#include "dgp_hero5.h"

#include <cassert>
#include <memory>

using namespace D_GP;

//----------------------------------------------------------------------//
GoPro* GoProFactory::createGoPro(GoProOwner* o,
				 CamModel model,
				 std::string& name)
{
  switch (model)
    {
    case CamModel::Hero5:
      return new GoProHero5(o, name);
    };

  // LOG the model type here since it was not picked up
  assert(false);
}
