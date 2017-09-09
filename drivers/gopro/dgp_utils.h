#ifndef DGP_UTILS_H
#define DGP_UTILS_H

#include "dgp_types.h"

#include <string>
#include <vector>

namespace D_GP
{
  class CmdToUrlConverter
  {
  public:
    // GoPro Hero5 connect command takes one parameter for the connection name
    // Parameters must be provided in the order that they appear in the url
    static std::string cmdToUrl(Cmd,
				CamModel,
				const std::vector<std::string>& params);
    static std::string cmdToUrl(Cmd,
				CamModel);
    static Cmd urlToCmd(const std::string& url,
			CamModel);
    //std::string cmdToMessage(Cmd); not used for now
    
  private:
    static std::string prependAddress(const std::string& cmd, CamModel model);
    
    static Cmd urlToCmdHero5(const std::string& url);
    static std::string cmdToUrlHero5(Cmd, const std::vector<std::string>& params);
    static bool validUrlParamsHero5(Cmd, const std::vector<std::string>& params);
  };
};

#endif
