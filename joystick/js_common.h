#ifndef JS_COMMON_H
#define JS_COMMON_H

#include <linux/joystick.h>

namespace JS
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

  struct AxisEvent
  {
    AxisEvent(const js_event &event);
    
    AxisID getID() { return d_id; }
    double getValue() { return d_value; }

    private:
    double d_value = 0.0; //from -1.0 to 1.0 
    AxisID d_id = AXIS_UNKOWN; 
    double d_min_value;
    double d_max_value;
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
