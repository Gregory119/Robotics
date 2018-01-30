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
  protected:
    Owner() = default;
    // Declaring the move special members implicitly deletes the copy special members
    Owner(Owner&&) = delete;
    Owner& operator=(Owner&&) = delete;
    // Move special member functions are not declared when desctructor is.
    ~Owner() = default; // must inherit and non-polymorphic
    
  private:
    friend class Config;
    virtual void handleError(Config*, Error, const std::string& msg) = 0;
    // Errors must be logged by the owner because they are not done internally.
    // This function is called on a zero timer from the constructor.
  };
  
 public:
  Config(Owner*, const std::string file_path); // absolute or relative path
  void setOwner(Owner* o) { d_owner = o; }

  // parseFile() must be called before any other functions for valid data
  void parseFile(); 
  
  bool hasError();
  
  P_WP::PinNum getPinNum(PinId);
  P_WP::PullMode getPinPullMode(PinId);

 private:
  void ownerHandleError(Error, const std::string& msg);
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

  const std::string d_file_path;
  
  PinConfig d_mode_pin;
  PinConfig d_trigger_pin;
  PinConfig d_connect_pin;

  Error d_error = Error::None;
};

#endif
