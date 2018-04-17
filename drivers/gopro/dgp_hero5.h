#ifndef DGP_HERO5_H
#define DGP_HERO5_H

#include "chttp_operations.h"
#include "cudp_client.h"
#include "dgp_gopro.h"
#include "kn_asiocallbacktimer.h"

#include <memory>

namespace D_GP
{
  // Automatically attempts to connect after construction
  // Wifi connection assumed by default. For a bluetooth connection create a GoProHero5BT class.
  // Accommodates multiple simultaneous requests by buffering them.
  
  class GoProHero5 final : public GoPro,
    C_HTTP::Operations::Owner,
    C_UDP::Client::Owner
  {
  public:
    GoProHero5(GoPro::Owner* o, const std::string& name);

  private:
    //GoPro
    void connect() override;
    void status() override; 
    void setMode(Mode) override;
    void setShutter(bool) override;
    void setBitRatePerSecond(unsigned) override;

    // The stream lasts about 6 seconds. It will not end if another start stream request is made within this time.
    void startLiveStream() override;
    void stopLiveStream() override;
    
    bool hasBufferedCmds() override;
    void cancelBufferedCmds() override; // will not stop a command currently being processed

  private:    
    // C_HTTP::Operations::Owner
    void handleFailed(C_HTTP::Operations*,
		      C_HTTP::OpError,
		      const std::string&) override;
    void handleResponse(C_HTTP::Operations*,
			C_HTTP::ResponseCodeNum,
			const std::vector<std::string>& headers,
			const std::vector<char>& body) override;

    // C_UDP::Client::Owner
    void handleConnected(C_UDP::Client*) override;
    void handleMessageSent(C_UDP::Client*) override;
    void handleFailed(C_UDP::Client*, C_UDP::Client::Error) override;
    
  private:
    void requestCmd(GoPro::Cmd);
    void internalStopLiveStream(); // does not notify owner
    void maintainStream();

    void ownerCmdFailed(GoPro::Cmd, GoPro::Error, const std::string&);
    
  private:
    std::unique_ptr<C_HTTP::Operations> d_http; 
    bool d_connected = false;

    std::string d_connect_name;
    std::list<GoPro::Cmd> d_cmd_reqs;
    GoPro::Cmd d_non = GoPro::Cmd::Unknown;

    std::unique_ptr<C_UDP::Client> d_udp_client;
    KERN::AsioCallbackTimer d_timer_stream = KERN::AsioCallbackTimer("D_GP::GoProHero5 - stream maintain timer.");
    bool d_is_streaming = false;
  };
};
#endif
