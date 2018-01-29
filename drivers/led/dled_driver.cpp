#include "dled_driver.h"

#include <fstream>

using namespace D_LED;

//----------------------------------------------------------------------//
Driver::Driver(const std::string dir)
  : d_dir(std::move(dir)),
    d_brightness_path(d_dir+"/brightness"),
    d_trigger_path(d_dir+"/trigger"),
    d_delay_on_path(d_dir+"/delay_on"),
    d_delay_off_path(d_dir+"/delay_off")
{}

//----------------------------------------------------------------------//
bool Driver::useMemory()
{
  std::fstream file(d_trigger_path);
  if (!file)
    {
      return false;
    }
  file << "mmc0" << std::endl;
  return true;
}

//----------------------------------------------------------------------//
bool Driver::turnOn()
{
  std::fstream file(d_brightness_path);
  if (!file)
    {
      return false;
    }
  file << "1" << std::endl;
  return true;
}

//----------------------------------------------------------------------//
bool Driver::turnOff()
{
  std::fstream file(d_brightness_path);
  if (!file)
    {
      return false;
    }
  file << "0" << std::endl;
  return true;
}

//----------------------------------------------------------------------//
bool Driver::flashOnOff(const std::chrono::milliseconds& delay_on,
			    const std::chrono::milliseconds& delay_off)
{
  std::fstream trig_file(d_trigger_path);
  if (!trig_file)
    {
      return false;
    }
  trig_file << "timer" << std::endl;

  std::fstream delay_off_file(d_delay_off_path);
  if (!delay_off_file)
    {
      return false;
    }
  delay_off_file << delay_off.count() << std::endl;

  std::fstream delay_on_file(d_delay_on_path);
  if (!delay_on_file)
    {
      return false;
    }
  delay_on_file << delay_on.count() << std::endl;
  return true;
}
