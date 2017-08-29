#ifndef DGP_HERO5_H
#define DGP_HERO5_H

#include "dgp_gopro.h"
#include "chttp_operations.h"

#include <memory>

namespace D_GP
{
  // Wifi connection assumed by default. For a bluetooth connection create a GoProHero5BT class.
  // Accommodates multiple simultaneous requests by buffering them.
  
  class GoProHero5 final : GoPro,
    C_HTTP::HttpOperationsOwner
  {
  public:
    explicit GoProHero5(GoProOwner* o);

    //connectWithName: Re-creates a http operations instance. Must be called once before using other commands.
    void connect() override;
    void setMode(Mode) override;
    void setShutter(bool) override;
    void startLiveStream() override;
    void stopLiveStream() override;

  private:
    // C_HTTP::HttpOperationsOwner
    void handleFailed(HttpOperations*,HttpOpError) override;
    void handleResponse(HttpOperations*,
			HttpResponseCode,
			const std::vector<std::string>& headers,
			const std::vector<char>& body) override;
    
  private:
    void cancel(); // stop all timers and reset values
    
  private:
    std::unique_ptr<C_HTTP::HttpOperations> d_http; 
    bool d_connected = false;

    // TO DO: list of buffered requests
    KERN::KernelTimer d_timer_connect_check; // STILL TO DO !!!!finish setting this up
    //KERN::KernelTimer d_timer_stream_check; // STILL TO DO !!!!finish setting this up: poll the stream to keep it up
  };
};
#endif
