#ifndef DGP_GOPRO_H
#define DGP_GOPRO_H

#include "dgp_types.h"

/*
  This is the gopro interface.

  - The concrete class should automatically attempt to connect after construction (using zero timer). 
  - The GoPro should automatically and continuously attempt to reconnect after a successful connection.
*/

#include <string>

namespace D_GP
{
  enum class GPError
  {
    Internal, // internal to the client of the gopro (this should not happen)
    Response, // gopro responded with error message
    ResponseData, // gopro response data was unexpected
    Timeout // a timeout occured after waiting for a gopro response
  };
	
  class GoPro;
  class GoProOwner // inherit privately
  {
  public:
    GoProOwner(const GoProOwner&) = delete;
    GoProOwner& operator=(const GoProOwner&) = delete;
    GoProOwner(GoProOwner&&) = delete;
    GoProOwner&& operator=(GoProOwner&&) = delete;

    // queued commands should be cleared on a failure
    virtual void handleCommandFailed(GoPro*, Cmd, GPError) = 0;
    virtual void handleCommandSuccessful(GoPro*, Cmd) = 0;
    
  protected:
    GoProOwner() = default;
    ~GoProOwner() = default;
  };
  
  class GoPro
  {
  public:
    virtual ~GoPro() = default;
    GoPro& operator=(const GoPro&) = default;
    GoPro(const GoPro&) = default;
    // move constructor & operator

    virtual void connect() = 0;
    virtual void status() = 0; 
    virtual void setMode(Mode) = 0;
    virtual void setShutter(bool) = 0;
    virtual void startLiveStream() = 0;
    virtual void stopLiveStream() = 0;
    virtual bool hasBufferedReqs() = 0;
    virtual void cancelBufferedCmds() = 0;
    
    const Status& getStatus() { return d_status; }
    
  protected:
    //only to be inherited
    explicit GoPro(GoProOwner* o);
    GoProOwner* d_owner = nullptr;

    Status d_status;
  };
};

#endif
