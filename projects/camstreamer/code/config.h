#ifndef CONFIG_H
#define CONFIG_H

#include "core_fileparammanager.h"
#include "core_owner.h"
#include "kn_asiocallbacktimer.h"
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
// mode_pin_num=<number>
// mode_pin_pull_mode=<up/down/none>
// trigger_pin_num=<number>
// trigger_pin_pull_mode=<up/down/none>
// -------------------------------------//

class Config
{  
 public:
  enum class Error
  {
    OpenFile,
      WifiSSID,
      WifiPassword,
      ModePinNum,
      ModePinPullMode,
      TriggerPinNum,
      TriggerPinPullMode,
      ConnectPinNum,
      ConnectPinPullMode,
      PinId
  };

  enum class PinId
  {
    Mode,
      Trigger
  };

  struct PinConfig
  {
    P_WP::PinNum num = P_WP::PinNum::Unknown;
    P_WP::PullMode pull_mode = P_WP::PullMode::Up;
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

  PinConfig getPinConfig(PinId);
  P_WP::PinNum getPinNum(PinId);
  P_WP::PullMode getPinPullMode(PinId);
  const std::string& getWifiSsid() const { return d_wifi_ssid; }
  const std::string& getWifiPassword() const { return d_wifi_pw; }

  bool hasError() { return d_has_error; }

 private:
  void ownerHandleError(Error, const std::string& msg);
  bool extractWifiSSID();
  bool extractWifiPw();
  bool extractPinNumber(PinConfig&,
			const std::string& pin_num_match,
			Error); // the error type is only used on an internal error condition
  bool extractPinPullMode(PinConfig&,
			  const std::string& pin_mode_match,
			  Error);
  
 private:
  Owner* d_owner = nullptr;

  std::unique_ptr<CORE::FileParamManager> d_file_param_man;
  
  std::string d_wifi_ssid;
  std::string d_wifi_pw;
  PinConfig d_mode_pin;
  PinConfig d_trigger_pin;

  bool d_has_error = false;

  KERN::AsioCallbackTimer d_zero_timer = KERN::AsioCallbackTimer("Config:: Zero timer");
};

#endif
