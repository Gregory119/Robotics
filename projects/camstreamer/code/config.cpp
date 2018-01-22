#include "config.h"

//----------------------------------------------------------------------//
Config::Config(std::string file_path)
{
  // read, parse, and validify textfile
  
}

//----------------------------------------------------------------------//
bool Config::hasError()
{
  return d_has_error;
}

//----------------------------------------------------------------------//
P_WP::PinNum Config::getPinNum(PinId id)
{
  switch (id)
    {
    case PinId::Mode:
      return d_mode_pin.num;
      
    case PinId::Trigger:
      return d_trigger_pin.num;
      
    case PinId::Connect:
      return d_connect_pin.num;
    }
}

//----------------------------------------------------------------------//
P_WP::PinNum Config::getPinPullMode(PinId id)
{
  switch (id)
    {
    case PinId::Mode:
      return d_mode_pin.pull_mode;
      
    case PinId::Trigger:
      return d_trigger_pin.pull_mode;
      
    case PinId::Connect:
      return d_connect_pin.pull_mode;
    }
}
