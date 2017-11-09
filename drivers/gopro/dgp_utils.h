#ifndef DGP_UTILS_H
#define DGP_UTILS_H

#include "dgp_types.h"
#include "dgp_gopro.h"

#include <string>
#include <vector>

namespace D_GP
{
  class Utils
  {
  public:
    // GoPro Hero5 connect command takes one parameter for the connection name
    // Parameters must be provided in the order that they appear in the url
    static std::string cmdToUrl(GoPro::Cmd,
				CamModel,
				const std::vector<std::string>& params);
    static std::string cmdToUrl(GoPro::Cmd,
				CamModel);
    static const std::string& ipAddr(CamModel);

    // returns an Unknown mode on failure
    static Mode extractModeStatus(int mode, int sub_mode);
    
  private:
    static std::string appendToAddress(const std::string& cmd, CamModel model);
    
    static std::string cmdToUrlHero5(GoPro::Cmd, const std::vector<std::string>& params);
    static bool validUrlParamsHero5(GoPro::Cmd, const std::vector<std::string>& params);
  };
};

#endif
