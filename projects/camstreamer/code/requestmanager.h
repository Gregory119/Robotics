#ifndef REQUESTMANAGER_H
#define REQUESTMANAGER_H

#include "config.h"

#include "core_owner.h"
#include "stateedgerequest.h"

class RequestManager final : Config::Owner,
  StateEdgeRequest::Owner
{
 public:
  enum class Error
  {
    Mode,
      Trigger,
      Shutdown,
      Restart
  };
  
 public:
  class Owner
  {
    OWNER_SPECIAL_MEMBERS(RequestManager);
    virtual void handleReqMode(RequestManager*) = 0;
    virtual void handleReqTrigger(RequestManager*) = 0;
    virtual void handleReqShutdown(RequestManager*) = 0;
    virtual void handleReqRestartPower(RequestManager*) = 0;
    virtual void handleError(RequestManager*,
			     Error,
			     const std::string& msg) = 0;
  };

 public:
  RequestManager(Owner*, std::string config_file_path);

  SET_OWNER();
  
  void start();
  
 private:
  // Config::Owner
  void handleError(Config*, Config::Error, const std::string& msg) override;

  // StateEdgeRequest::Owner
  void handleRequest(StateEdgeRequest*, DurationTrigger) override;
  void handleError(StateEdgeRequest*, const std::string&) override;
  
 private:
  CORE::Owner<Owner> d_owner;
  std::unique_ptr<Config> d_config;

  std::unique_ptr<StateEdgeRequest> d_mode_req;
  std::unique_ptr<StateEdgeRequest> d_trigger_req;
  std::unique_ptr<StateEdgeRequest> d_power_req;
};

#endif
