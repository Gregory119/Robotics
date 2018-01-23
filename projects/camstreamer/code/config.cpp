#include "config.h"

#include <fstream>

//----------------------------------------------------------------------//
Config::Config(std::string file_path)
{
  // read, parse, and validify textfile
  ifstream file(file_path.c_str());

  if (file.fail())
    {
      assert(false);
      // !!!!!!!!!!start zero fail timer
      return;
    }

  // Expected file formate:
  // mode_pin_num=<number>
  // mode_pin_pull_mode=<up/down/none>
  // trigger_pin_num=<number>
  // trigger_pin_pull_mode=<up/down/none>
  // connect_pin_num=<number>
  // connect_pin_pull_mode=<up/down/none>
  
  // Note: these pins cannot be the shutdown pin (this pin is still to be selected)
  
  std::string line;
  std::getline(file, line);
  if (line.find("mode_pin_num=") != 0)
    {
      // incorrect format
      assert(false);
      // zero fail timer
      return;
    }
  
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
