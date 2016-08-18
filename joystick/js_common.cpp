#include "js_common.h"
#include "assert.h"

using namespace JS;

Map s_stick_axis_map(32767,-32767,1.0,-1.0);
Map s_lever_axis_map(32767,-32767,1.0,0.0);

//----------------------------------------------------------------------//
double JS::mapFromTo(const Map &map, int orig_val, bool flip_axis)
{
  //check value is in range
  assert(orig_val >= map.d_orig_min);
  assert(orig_val <= map.d_orig_max);

  double new_val = static_cast<double>(orig_val - map.d_orig_min)/(map.d_orig_max - map.d_orig_min)*
    (map.d_new_max - map.d_new_min) + map.d_new_min;

  if (flip_axis)
    {
      new_val = (map.d_new_max - new_val) + map.d_new_min;
    }

  return new_val;
}

//----------------------------------------------------------------------//
ButtonEvent::ButtonEvent(const js_event &event)
  : d_value(event.value),
    d_id(static_cast<ButtonID>(event.number))
{}

//----------------------------------------------------------------------//
AxisEvent::AxisEvent(const js_event &event)
  : d_id(static_cast<AxisID>(event.number)),
    d_min_value(s_lever_axis_map.d_new_min),
    d_max_value(s_lever_axis_map.d_new_max)
{
  if ((d_id == RT) ||
      (d_id == LT))
    {
      d_value = mapFromTo(s_lever_axis_map, event.value);
    }
  else if ((d_id == Y1) ||
	   (d_id == Y2))
    {
      bool flip_axis = true;
      d_value = mapFromTo(s_stick_axis_map, event.value, flip_axis);
    }
  else
    {
      d_value = mapFromTo(s_stick_axis_map, event.value);
    }
}
