#ifndef DGP_HERO5_H
#define DGP_HERO5_H

#include "chttp_operations.h"
#include "dgp_gopro.h"
#include "kn_timer.h"

#include <memory>

namespace D_GP
{
  // Automatically attempts to connect after construction
  // Wifi connection assumed by default. For a bluetooth connection create a GoProHero5BT class.
  // Accommodates multiple simultaneous requests by buffering them.
  
  class GoProHero5 final : public GoPro,
    C_HTTP::HttpOperationsOwner,
    KERN::KernelTimerOwner
  {
  public:
    explicit GoProHero5(GoProOwner* o, const std::string& name);

    //GoPro
    void setMode(Mode) override;
    void setShutter(bool) override;
    void startLiveStream() override;
    void stopLiveStream() override;

  private:
    //GoPro
    void connect() override;
    void setName(const std::string& name) override;
    
    // C_HTTP::HttpOperationsOwner
    void handleFailed(C_HTTP::HttpOperations*, C_HTTP::HttpOpError) override;
    void handleResponse(C_HTTP::HttpOperations*,
			C_HTTP::HttpResponseCode,
			const std::vector<std::string>& headers,
			const std::vector<char>& body) override;

    // KERN::KernelTimer
    bool handleTimeOut(const KERN::KernelTimer&) override;
    
  private:
    void cancel(); // stop all timers and reset values

  private:
    struct Request
    {
      Cmd cmd;
      std::vector<std::string> params;
    };
    
  private:
    std::unique_ptr<C_HTTP::HttpOperations> d_http; 
    bool d_connected = false;
    std::string d_name;

    std::string d_connect_name;
    KERN::KernelTimer d_timer_connect_check;
    //KERN::KernelTimer d_timer_stream_check; // STILL TO DO !!!!finish setting this up: poll the stream to keep it up
  };
};
#endif
