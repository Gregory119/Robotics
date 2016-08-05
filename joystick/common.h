#ifndef COMMON_H
#define COMMON_H

#include <linux/joystick.h>

namespace JS_CO
{
  enum ButtonID
  {
    A,
    B,
    X,
    Y,
    LB,
    RB,
    BACK,
    START,
    GUIDE,
    TL,
    TR,
    BUTTON_UNKNOWN
  };

  enum AxisID
  {
    X1,
    Y1,
    X2,
    Y2,
    RT,
    LT,
    AXIS_UNKOWN
  };

  struct Map
  {
  Map(int o_max, int o_min, double n_max, double n_min)
  : d_orig_max(o_max),
      d_orig_min(o_min),
      d_new_max(n_max),
      d_new_min(n_min)
    {}

    int d_orig_max;
    int d_orig_min;
    double d_new_max;
    double d_new_min;
  };
  
  double mapFromTo(const Map &map, int orig_val, bool flip_axis=false);
  //flip axis will make the new max value correspond to the original min value and make the new min value correspond to the original max value.

  struct AxisEvent
  {
    AxisEvent(const js_event &event);
    
    AxisID getID() { return d_id; }
    double getValue() { return d_value; }

    private:
    double d_value = 0.0; //from -1.0 to 1.0 
    double d_min_value;
    double d_max_value;
    AxisID d_id = AXIS_UNKOWN; 
  };

  struct ButtonEvent
  {
    ButtonEvent(const js_event &event);
    
    ButtonID getID() { return d_id; }
    bool getValue() { return d_value; }

    private:
    bool d_value = false; 
    ButtonID d_id = BUTTON_UNKNOWN; 
  };
};
#endif
