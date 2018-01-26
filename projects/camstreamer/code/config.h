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
      ConnectPinPullMode
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
    Owner(const Owner&) = delete;
    Owner& operator=(const Owner&) = delete;
    // move special members are not declared when the destructor is
    
  protected:
    ~Owner() = default; // only inherited but non-polymorphic
    
  private:
    friend class Config;
    virtual void handleError(Config*, Error, const std::string& msg) = 0;
  };
  
 public:
  Config(Owner*, const std::string& file_path); // absolute or relative path

  bool hasError();
  
  P_WP::PinNum getPinNum(PinId);
  P_WP::PinNum getPinPullMode(PinId);

 private:
  void singleShotError(Error, const std::string& message);
  bool extractPinNumber(PinConfig&,
			ifstream& file,
			const std::string& pin_num_text,
			Error); // the error type is only use on an internal error condition
  bool extractPinPullMode(PinConfig&,
			  ifstream& file,
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
