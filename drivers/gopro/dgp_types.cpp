#include "dgp_types.h"

using namespace D_GP;

namespace D_GP
{
  const std::string& g_ip_url = "http://10.5.5.9";
  const std::string& g_bacpac = "/bacpac/cv";
  const std::string& g_wifipair = "/gp/gpControl/command/wireless/pair/complete";
  const std::string& g_mode = "/gp/gpControl/command/mode";
  const std::string& g_shutter = "/gp/gpControl/command/shutter";
};

//----------------------------------------------------------------------//
std::string D_GP::CreateCmdUrl(const std::string& cmd)
{
  return g_ip_url + cmd;
}


