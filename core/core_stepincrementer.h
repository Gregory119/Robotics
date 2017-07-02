#ifndef CORE_STEPINCREMENTER_H
#define CORE_STEPINCREMENTER_H

#include <functional>

namespace CORE
{
  class StepIncrementer
  {
  public:
    StepIncrementer(double abs_slope, int time_step_ms);
    int stepIncrement(int input) const; // returns the input on overflow and asserts in debug
    int stepDecrement(int input) const; // returns the input on overflow and asserts in debug
    int getIncRes() const { return d_inc_resolution; }

  private:
    double d_abs_slope = 0;
    int d_time_step_ms = 0;
    int d_inc_resolution = 0;
  };
};

#endif
