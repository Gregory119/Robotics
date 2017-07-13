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

  enum class Request
  {
    Connect,
    Picture,
    MultiShot,
    StartStopRec
  };
  
  enum class Mode
  {
    Disconnected,
    Connected,
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
  std::string cmdToString(Cmd);
  std::string reqToString(Request);
  
  extern const std::string& g_ip_url;
  extern const std::string& g_bacpac;
  extern const std::string& g_wifipair;
  extern const std::string& g_mode;
  extern const std::string& g_shutter;
};

#endif
