#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <string>

namespace Utils
{
  std::string CpuInfo();
  std::string CpuInfoNoSerial();
  std::string KernelInfo();
  std::chrono::milliseconds UpTime();
  std::string StrFile(const std::string& filename);
};

#endif
