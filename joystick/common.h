#ifndef TYPES_H
#define TYPES_H

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
    TR
  };

  enum AxesID
  {
    X1,
    Y1,
    X2,
    Y2,
    RT,
    LT
  };

  js_event {
    __u32 time;	// event timestamp in milliseconds 
    __s16 value;	// value 
    __u8 type;	// event type 
    __u8 number;	// axis/button number 
  }

  struct Map
  {
  Map(int o_max, int o_min, double n_max, double n_min)
  : orig_max(o_max),
      orig_min(o_min),
      new_max(n_max),
      new_min(n_min)
    {}

    int orig_max;
    int orig_min;
    double new_max;
    double new_min;
  };
  
  int mapFromTo(const Map &map, int orig_val, bool flip_axis=false);
  //flip axis will make the new max value correspond to the original min value and make the new min value correspond to the original max value.
  
  struct AxisEvent
  {
    AxisEvent(const js_event &event)
    
    AxesID getID() { return id; }
    double getValue() { return value; }

    private:
    double value = 0.0; //from -1.0 to 1.0 
    AxesID id = AXIS_UNKOWN; 
  };
};
#endif
