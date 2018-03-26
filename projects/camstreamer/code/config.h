#ifndef CONFIG_H
#define CONFIG_H

#include "core_fileparammanager.h"
#include "core_owner.h"
#include "kn_asiocallbacktimer.h"
#include "types.h"
#include "wp_pins.h"

#include <string>

// Expected file format:
// - the parameters can be in any order
// - the first occurence of the parameter will be used
// - pins numbers are those used by WiringPi
// - do NOT use quotation marks around the wifi ssid or wifi password
// -------------------------------------//
// wifi_ssid=<ssid>
// wifi_pw=<password>
// ppm_pin_num=<number/float>
// mode_req_num=<pin_number/ppm_channel>
// mode_req_mode=<up/down/float/pwm/ppm>
// trigger_req_num=<pin_number/ppm_channel>
// trigger_req_mode=<up/down/float/pwm/ppm>
// power_req_num=<pin_number/ppm_channel>
// power_req_mode=<up/down/float/pwm/ppm>
// -------------------------------------//

class Config
{  
 public:
  enum class Error
  {
    OpenFile,
      WifiSSID,
      WifiPassword,
      ModeReqNum,
      ModeReqMode,
      TriggerReqNum,
      TriggerReqMode,
      PowerReqNum,
      PowerReqMode,
      Request,
      CombinedControl, // request and mode control on the same pin/channel
      DuplicateReqNum
  };

  enum class Request
  {
    Mode,
      Trigger,
      Power
  };

  struct ReqConfig
  {
    int num = -1;
    ReqMode mode = ReqMode::Unknown;
  };

 public:
  class Owner
  {
    OWNER_SPECIAL_MEMBERS(Config);
    // Errors must be logged by the owner because they are not done internally.
    // Messages do not have a new line character at the end.
    virtual void handleError(Config*, Error, const std::string& msg) = 0;
  };
  
 public:
  Config(Owner*, const std::string file_path); // absolute or relative path
  void setOwner(Owner* o);

  // parseFile() must be called before any other functions
  void parseFile(); 

  ReqConfig getReqConfig(Request);
  int getReqNum(Request);
  ReqMode getReqMode(Request);
  const std::string& getWifiSsid() const { return d_wifi_ssid; }
  const std::string& getWifiPassword() const { return d_wifi_pw; }

  bool hasError() { return d_has_error; }

 private:
  void ownerHandleError(Error, const std::string& msg);
  bool extractWifiSSID();
  bool extractWifiPw();
  bool extractReqNumber(ReqConfig&,
			const std::string& req_num_match,
			Error); // the error type is only used on an internal error condition
  bool extractReqMode(ReqConfig&,
		      const std::string& req_mode_match,
		      Error);
  void checkDuplicateReqNums();
  
 private:
  Owner* d_owner = nullptr;

  std::unique_ptr<CORE::FileParamManager> d_file_param_man;
  
  std::string d_wifi_ssid;
  std::string d_wifi_pw;
  ReqConfig d_mode_req_conf;
  ReqConfig d_trigger_req_conf;
  ReqConfig d_power_req_conf;

  bool d_has_error = false;

  KERN::AsioCallbackTimer d_zero_timer = KERN::AsioCallbackTimer("Config:: Zero timer");
};

#endif
