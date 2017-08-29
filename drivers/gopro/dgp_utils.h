#ifndef DGP_UTILS_H
#define DGP_UTILS_H

#include "dgp_types.h"

#include <string>

namespace D_GP
{
  class CmdConverter
  {
  public:
    static std::string cmdToUrl(Cmd, CamModel);
    static Cmd urlToCmd(const std::string&, CamModel);
    //std::string cmdToMessage(Cmd); not used for now
    
  private:
    static std::string createCmdUrl(const std::string& cmd, CamModel model);
    
    static Cmd urlToCmdHero5(const std::string&);
    static std::string cmdToUrlHero5(Cmd);
  };
};

#endif
