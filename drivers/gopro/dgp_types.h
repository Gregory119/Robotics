#ifndef DGP_TYPES_H
#define DGP_TYPES_H

#include <string>

namespace D_GP
{
  enum class Cmd
  {
    Connect,
    SetMode,
    SetShutter
  };

  enum class Mode
  {
    Photo,
    Video
  };

  enum class ControlType
  {
    Simple,  //Synchronous
    Fast   //Asynchronous
  };
  
  std::string CreateCmdUrl(const std::string& cmd);
  std::string cmdToString(Cmd);
  
  extern const std::string& g_ip_url;
  extern const std::string& g_bacpac;
  extern const std::string& g_wifipair;
  extern const std::string& g_mode;
  extern const std::string& g_shutter;
};

#endif
