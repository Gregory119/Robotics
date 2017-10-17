#ifndef DGP_HERO5_H
#define DGP_HERO5_H

#include "chttp_operations.h"
#include "dgp_gopro.h"
#include "kn_callbacktimer.h"

#include <memory>

namespace D_GP
{
  // Automatically attempts to connect after construction
  // Wifi connection assumed by default. For a bluetooth connection create a GoProHero5BT class.
  // Accommodates multiple simultaneous requests by buffering them.
  
  class GoProHero5 final : public GoPro,
    C_HTTP::HttpOperationsOwner
  {
  public:
    GoProHero5(GoPro::Owner* o, const std::string& name);

    //GoPro
    void connect() override;
    void status() override; 
    void setMode(Mode) override;
    void setShutter(bool) override;

    // The stream lasts about 6 seconds. It will not end if another start stream request is made within this time.
    void startLiveStream() override;
    void stopLiveStream() override;
    
    bool hasBufferedReqs() override;
    void cancelBufferedCmds() override;

  private:    
    // C_HTTP::HttpOperationsOwner
    void handleFailed(C_HTTP::HttpOperations*, C_HTTP::HttpOpError) override;
    void handleResponse(C_HTTP::HttpOperations*,
			C_HTTP::HttpResponseCode,
			const std::vector<std::string>& headers,
			const std::vector<char>& body) override;

  private:
    void requestCmd(GoPro::Cmd);
    
  private:
    std::unique_ptr<C_HTTP::HttpOperations> d_http; 
    bool d_connected = false;

    std::string d_connect_name;
    std::list<GoPro::Cmd> d_cmd_reqs;
    GoPro::Cmd d_non = GoPro::Cmd::Unknown;

    KERN::CallbackTimer d_timer_stream;
    KERN::CallbackTimer d_timer_init_failed;
  };
};
#endif
