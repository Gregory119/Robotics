#ifndef CONFIG_H
#define CONFIG_H

#include "kn_asiocallbacktimer.h"
#include "wp_pins.h"

#include <string>

class Config
{  
 public:
  enum class Error
  {
    None,
    OpenFile,
    ModePinNum,
      ModePinPullMode,
      TriggerPinNum,
      TriggerPinPullMode,
      ConnectPinNum,
      ConnectPinPullMode,
      PinIdForPullMode, // Should never happen
      PinIdForPinNum // Should never happen
  };

  enum class PinId
  {
    Mode,
      Trigger,
      Connect
  };

  struct PinConfig
  {
    P_WP::PinNum num = P_WP::PinNum::Unknown;
    P_WP::PullMode pull_mode = P_WP::PullMode::Up;
  };

 public:
  class Owner
  {
  public:
    Owner() = default;
    Owner(const Owner&) = delete;
    Owner& operator=(const Owner&) = delete;
    // move special members are not declared when the destructor is
    
  protected:
    ~Owner() = default; // only inherited but non-polymorphic
    
  private:
    friend class Config;
    virtual void handleError(Config*, Error, const std::string& msg) = 0;
    // Errors must be logged by the owner because they are not done internally.
    // This function is called on a zero timer in the constructor.
  };
  
 public:
  Config(Owner*, const std::string& file_path); // absolute or relative path

  bool hasError();
  
  Error getError();
  const std::string& getErrorMsg();
  
  P_WP::PinNum getPinNum(PinId);
  P_WP::PullMode getPinPullMode(PinId);

 private:
  void ownerHandleError(Error, const std::string& msg);
  void singleShotError(Error, const std::string& msg);
  bool extractPinNumber(PinConfig&,
			std::ifstream& file,
			const std::string& pin_num_text,
			Error); // the error type is only use on an internal error condition
  bool extractPinPullMode(PinConfig&,
			  std::ifstream& file,
			  const std::string& pin_mode_text,
			  Error);
  
 private:
  Owner* d_owner = nullptr;
  
  PinConfig d_mode_pin;
  PinConfig d_trigger_pin;
  PinConfig d_connect_pin;

  Error d_error = Error::None;
  std::string d_error_msg;

  KERN::AsioCallbackTimer d_zero_fail_timer;
};

#endif
