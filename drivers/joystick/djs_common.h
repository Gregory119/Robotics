#ifndef DJS_COMMON_H
#define DJS_COMMON_H

#include <linux/joystick.h>
#include <cstdint>

namespace D_JS
{
  //xboxdrv
  /*
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
  //xboxdrv
  enum AxisID
  {
    X1,
    Y1,
    X2,
    Y2,
    RT,
    LT,
    AXIS_UNKOWN
    };*/

  //xpad
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
  //xpad
  enum AxisID
  {
    X1,
    Y1,
    LT,
    X2,
    Y2,
    RT,
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

  typedef js_event JSEvent;

  struct JSEventMinimal
  {
    uint16_t time_ms = 0;
    uint8_t value = 0;
    uint8_t type = 0;
    uint8_t number = 0;
  };

  extern const uint8_t axis_max;
  extern const uint8_t axis_min;
};
#endif
