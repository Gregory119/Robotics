#include "js_common.h"
#include "utl_mapping.h"

using namespace JS;
using namespace UTIL;

static const Map s_stick_axis_map(32767,-32767,1.0,-1.0);
static const Map s_lever_axis_map(32767,-32767,1.0,0.0);

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
