#include "utils.h"

#include <cassert>
#include <fstream>

using namespace Utils;

//----------------------------------------------------------------------//
std::string Utils::CpuInfo()
{
  return StrFile("/proc/cpuinfo");
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

//----------------------------------------------------------------------//
std::string Utils::KernelInfo()
{
  return StrFile("/proc/version");
}

//----------------------------------------------------------------------//
std::chrono::milliseconds Utils::UpTime()
{
  double sec_d = 0.0;
  std::ifstream file("/proc/uptime");
  if (file.fail())
    {
      assert(false);
      return std::chrono::milliseconds(0);
    }
  
  file >> sec_d;

  unsigned long sec = static_cast<unsigned long>(sec_d*1000);
  return std::chrono::milliseconds(sec);
}

//----------------------------------------------------------------------//
std::string Utils::StrFile(const std::string& filename)
{
  std::ifstream file(filename);
  if (file.fail())
    {
      assert(false);
      return std::string();
    }

  std::string out;
  std::getline(file, out, file.widen(EOF));

  return out;
}
