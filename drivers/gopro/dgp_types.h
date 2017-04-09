#ifndef DGP_TYPES_H
#define DGP_TYPES_H

#include <string>

namespace D_GP
{
  enum class Cmd
  {
    SetMode,
    SetShutter
  };

  enum class Request
  {
    Picture,
    MultiShot,
    StartStopRec
  };
  
  enum class Mode
  {
    Photo,
    Video,
    MultiShot
  };

  enum class ControlType
  {
    Simple,  //Synchronous
    Fast   //Asynchronous
  };
  
  std::string CreateCmdUrl(const std::string& cmd);
  
  extern const std::string& g_ip_url;
  extern const std::string& g_bacpac;
  extern const std::string& g_wifipair;
  extern const std::string& g_mode;
  extern const std::string& g_shutter;
};

#endif
