#ifndef CORE_STEPINCREMENTER_H
#define CORE_STEPINCREMENTER_H

#include <functional>

namespace CORE
{
  class StepIncrementer
  {
  public:
    StepIncrementer(double abs_slope, int time_step_ms);
    void setValue(int val) { d_val = val; }
    int getValue() { return d_val; }
    int stepIncrement();
    int stepDecrement();
    int getIncRes() const;

  private:
    double d_val = 0;
    double d_abs_slope = 0;
    int d_time_step_ms = 0;
    int d_inc_resolution = 0;
  };
};

#endif
