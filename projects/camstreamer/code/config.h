#ifndef CONFIG_H
#define CONFIG_H

class Config
{  
 public:
  enum class Error
  {
    File,
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
    virtual void handleConfigError(Error) = 0;
  };
  
 public:
  Config(std::string file_path); // absolute or relative path

  bool hasError();
  
  P_WP::PinNum getPinNum(PinId);
  P_WP::PinNum getPinPullMode(PinId);

 private:
  PinConfig d_mode_pin;
  PinConfig d_trigger_pin;
  PinConfig d_connect_pin;

  bool d_has_error = true;

  // !!!!!!!!!!!add zero fail timer
};

#endif
