#ifndef TYPES_H
#define TYPES_H

#include "wp_pins.h"

enum class ReqMode
  {
    Up,
      Down,
      Float,
      Pwm,
      Ppm,
      Unknown
      };

//----------------------------------------------------------------------//
bool isValidPullMode(ReqMode mode)
{
  switch (mode)
    {
    case ReqMode::Up:
    case ReqMode::Down:
    case ReqMode::Float:
      return true;
      
    case ReqMode::Pwm:
    case ReqMode::Ppm:
    case ReqMode::Unknown:
    }
  return false;
};

#endif
