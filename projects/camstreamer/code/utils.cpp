#include "utils.h"

#include <cassert>
#include <fstream>

//----------------------------------------------------------------------//
std::string Utils::CpuInfo()
{
  std::ifstream file("/proc/cpuinfo");
  if (file.fail())
    {
      assert(false);
      return std::string();
    }

  std::string out;
  std::getline(file, out, file.widen(EOF));

  return out;
}

//----------------------------------------------------------------------//
std::string Utils::CpuInfoNoSerial()
{
  std::string info = Utils::CpuInfo();

  size_t ser_start = info.find("Serial");
  if (ser_start == std::string::npos)
    {
      assert(false);
      return std::string();
    }
  info.erase(info.begin()+ser_start-1, info.end());

  return info;
}
