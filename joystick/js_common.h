#ifndef JS_COMMON_H
#define JS_COMMON_H

#include <linux/joystick.h>
#include <cstdint>

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

  enum EventType
  {
    BUTTON = 1,
    AXIS
  };

  //linux joystick event definintion
//struct js_event {
  //	__u32 time;	/* event timestamp in milliseconds */
  //	__s16 value;	/* value */
  ///	__u8 type;	/* event type */
  //	__u8 number;	/* axis/button number */
  //};

  struct JSEvent
  {
    uint32_t time = 0;
    int16_t value = 0;
    uint8_t type = 0;
    uint8_t number = 0;
  };

  struct JSEventMinimal
  {
    uint16_t time_ms = 0;
    int8_t value = 0;
    uint8_t type = 0;
    uint8_t number = 0;
  };

  //will not need this in the future
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

  //will not need this in the future
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
