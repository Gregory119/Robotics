#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include "config.h"

#include "kn_asiocallbacktimer.h"
#include "core_owner.h"
#include "crc_stateppmreader.h"
#include "stateedgerequest.h"

class RequestManager final : Config::Owner,
  StateEdgeRequest::Owner
{
 public:
  enum class Error
  {
    NextModeReq,
      TriggerReq,
      PowerReq,
      ConvertReqModeToPullMode,
      PpmPinNum,
      ReqMode,
      Config
  };
  
 public:
  class Owner
  {
    OWNER_SPECIAL_MEMBERS(RequestManager);
    virtual void handleReqNextMode(RequestManager*) = 0;
    virtual void handleReqTrigger(RequestManager*) = 0;
    virtual void handleReqShutdown(RequestManager*) = 0;
    virtual void handleReqReboot(RequestManager*) = 0;
    virtual void handleError(RequestManager*,
			     Error,
			     const std::string& msg) = 0;
  };

 public:
  RequestManager(Owner*,
		 std::string config_file_path);

  SET_OWNER();
  
  void start();
  void stop();

 private:
  enum class Mode
  {
    SeparateModeAndTrigger,
      CombinedModeAndTrigger
      };
    
 private:
  // Config::Owner
  void handleError(Config*, Config::Error, const std::string& msg) override;

  // StateEdgeRequest::Owner
  void handleRequest(StateEdgeRequest*, StateEdgeRequest::Trigger) override;
  void handleError(StateEdgeRequest*, const std::string&) override;

 private:
  std::unique_ptr<StateEdgeRequest> createRequest(Config::Request);
  void ownerError(Error, const std::string&);
  
 private:
  CORE::Owner<Owner> d_owner;
  std::unique_ptr<Config> d_config;
  Mode d_mode = Mode::SeparateModeAndTrigger;

  bool d_started = false;
  
  std::unique_ptr<StateEdgeRequest> d_next_mode_req;
  std::unique_ptr<StateEdgeRequest> d_trigger_req;
  std::unique_ptr<StateEdgeRequest> d_power_req;

  std::unique_ptr<C_RC::StatePpmReader> d_stateppmreader;

  KERN::AsioCallbackTimer d_err_timer = KERN::AsioCallbackTimer("RequestManager - Error timer.");
};

#endif
