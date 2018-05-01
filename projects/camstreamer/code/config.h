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
// - all parameters must be valid to start the program
// -------------------------------------//
/*
wifi_ssid=<ssid>
wifi_pw=<password>
next_mode_req_num=<pin_number/ppm_channel>
next_mode_req_mode=<up/down/float/pwm/ppm>
trigger_req_num=<pin_number/ppm_channel>
trigger_req_mode=<up/down/float/pwm/ppm>
power_req_num=<pin_number/ppm_channel>
power_req_mode=<up/down/float/pwm/ppm>
ppm_pin_num=<number>
pwm_pulse_high_us=<number>
pwm_pulse_low_us=<number>
*/
// -------------------------------------//

class Config
{  
 public:
  enum class Error
  {
    WifiSSID,
      WifiPassword,
      NextModeReqNum,
      NextModeReqMode,
      NextModeReqConfig,
      TriggerReqNum,
      TriggerReqMode,
      TriggerReqConfig,
      PowerReqNum,
      PowerReqMode,
      PowerReqConfig,
      RequestEnum,
      CombinedControl, // request and mode control on the same pin/channel
      DuplicateReqNum,
      PwmHigh,
      PwmLow,
      PwmHighAndLow,
      PpmPinNum,
      InterruptCount,
      Internal
  };

  enum class Request
  {
    NextMode,
      Trigger,
      Power
  };

  struct ReqConfig
  {
    bool isValid();
    const std::string& getErrMsg() { return err_msg; }
    
    int num = -1;
    ReqMode mode = ReqMode::Unknown;
    std::string err_msg;
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
  Config(Owner*, std::string file_path); // absolute or relative path
  SET_OWNER();

  // parseFile() must be called before any other functions
  void parseFile();
  std::string str();

  ReqConfig getReqConfig(Request);
  int getReqNum(Request);
  ReqMode getReqMode(Request);
  const std::string& getWifiSsid() const { return d_wifi_ssid; }
  const std::string& getWifiPassword() const { return d_wifi_pw; }
  int getPwmHighUs() { return d_pwm_high_us; }
  int getPwmLowUs() { return d_pwm_low_us; }

  P_WP::PinNum getPpmPinNum() { return d_ppm_pin_num; }
  static bool getPullMode(ReqMode, P_WP::PullMode&);
  
  bool hasError() { return d_has_error; }
  
 private:
  void extractWifiSSID();
  void extractWifiPw();
  void extractPpmPinNum();
  void extractReqNumber(const std::string& req_num_match,
			Error,
			ReqConfig&); // the error type is only used on an internal error condition
  void extractReqMode(const std::string& req_mode_match,
		      Error,
		      ReqConfig&);
  void extractPwmPulse(const std::string& pulse_match,
		       Error,
		       int&);
  void checkDuplicateReqNums();
  void checkInterruptCount();
  void checkPwmPulses();
  bool areConflictingReqModes(ReqMode, ReqMode); // both use pins (digital or pwm) or ppm channels
  bool usesPin(ReqMode);
  bool usesChannel(ReqMode);
  bool usesInterrupt(ReqMode);
  void ownerHandleError(Error, const std::string& msg);
  
 private:
  CORE::Owner<Owner> d_owner;

  std::unique_ptr<CORE::FileParamManager> d_file_param_man;
  
  std::string d_wifi_ssid;
  std::string d_wifi_pw;
  ReqConfig d_next_mode_req_conf;
  ReqConfig d_trigger_req_conf;
  ReqConfig d_power_req_conf;
  P_WP::PinNum d_ppm_pin_num = P_WP::PinNum::Unknown;
  int d_pwm_high_us = -1;
  int d_pwm_low_us = -1;

  bool d_has_error = false;

  KERN::AsioCallbackTimer d_zero_timer = KERN::AsioCallbackTimer("Config:: Zero timer");
};

#endif
