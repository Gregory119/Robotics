#include "core_stepincrementer.h"

#include <cassert>
#include <cmath>
#include <iostream>

using namespace CORE;

//----------------------------------------------------------------------//
StepIncrementer::StepIncrementer(double abs_slope, int time_step_ms)
  : d_abs_slope(abs_slope),
    d_time_step_ms(time_step_ms),
    d_inc_resolution(abs_slope*time_step_ms)
{
  assert(abs_slope > 0);
  assert(time_step_ms > 0);
}

//----------------------------------------------------------------------//
int StepIncrementer::stepIncrement()
{
  d_val += d_abs_slope * d_time_step_ms;
  return d_val;
}

//----------------------------------------------------------------------//
int StepIncrementer::stepDecrement()
{
  d_val -= d_abs_slope * d_time_step_ms;
  return d_val;
}

//----------------------------------------------------------------------//
int StepIncrementer::getIncRes() const
{
  assert(d_inc_resolution > 0); // increase the time step or decrease the slope
  return d_inc_resolution;
}
