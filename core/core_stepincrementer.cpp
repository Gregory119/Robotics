#include "core_stepincrementer.h"

#include <cassert>

using namespace CORE;

//----------------------------------------------------------------------//
StepIncrementer::StepIncrementer(int abs_slope, int time_step_ms)
  : d_abs_slope(abs_slope),
    d_time_step_ms(time_step_ms),
    d_inc_resolution(abs_slope*time_step_ms)
{}

//----------------------------------------------------------------------//
int StepIncrementer::stepIncrement(int input) const
{
  int ret = input + d_abs_slope * d_time_step_ms;
  if (ret < input) // overflow
    {
      assert(false);
      return input; // limit to input
    }

  if (ret == input) // increment too small
    {
      assert(false);
      return input; // limit to input
    }
  
  return ret;
}

//----------------------------------------------------------------------//
int StepIncrementer::stepDecrement(int input) const
{
  int ret = input - d_abs_slope * d_time_step_ms;
  if (ret > input) // overflow
    {
      assert(false);
      return input; // limit to input
    }

  if (ret == input) // increment too small
    {
      assert(false);
      return input; // limit to input
    }
  
  return ret;
}
