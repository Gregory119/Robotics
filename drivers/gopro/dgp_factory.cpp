#include "dgp_factory.h"

#include "dgp_simplegopro.h"

#include <cassert>
#include <memory>

using namespace D_GP;

//----------------------------------------------------------------------//
GoPro* GoProFactory::createGoPro(ControlType type, GoProOwner* o)
{
  switch (type)
    {
    case ControlType::Simple:
      return new SimpleGoPro(o);
      break;

    case ControlType::Fast:
      assert(false); //for now
      break;

    default:
      assert(false);
    };
}
