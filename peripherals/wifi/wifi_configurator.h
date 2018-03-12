#ifndef WIFI_CONFIGURATOR_H
#define WIFI_CONFIGURATOR_H

#include "core_fileparammanager.h"
#include "core_owner.h"
#include "kn_asiocallbacktimer.h"

#include <memory>
#include <string>

// Currently, this class can only be used with a OS that has systemd. A future change would be to accommodate other systems types that manage the wifi configuration.

// Expected file format:
// - the parsing method allows for the the ssid and psk to be in any order
// -------------------------------------//
// -----other settings-----
//    ssid="<ssid>"
//    psk="<password>"
// -----other settings-----
// -------------------------------------//
namespace P_WIFI
{
  class Configurator final
  {
  public:
    enum class Error
    {
      OpenFile,
	SSID,
	Password
	};

  public:
    class Owner
    {
      OWNER_SPECIAL_MEMBERS(Configurator);
      // Error messages do not include a new line character at the end
      // These errors do not show whether the configuration values are not what they should be.
      virtual void handleError(Configurator*, Error, const std::string& msg) = 0;
    };
  
  public:
    Configurator(Owner*, std::string file_path); // commonly used with the systemd wpa_supplicant.conf file
    void setOwner(Owner* o);
    
    // The cached parameters are cleared before parsing.
    void parseFile();
  
    const std::string& getSsid() { return d_ssid; }
    const std::string& getPassword() { return d_pw; }

    // Set commands can be used without parsing first.
    void setSsid(const std::string&);
    void setPassword(const std::string&);
    void setSsidWithQuotes(const std::string&);
    void setPasswordWithQuotes(const std::string&);

    bool hasError() { return d_has_error; }
  
  private:
    bool extractSSID();
    bool extractPassword();
    void ownerHandleError(Error, const std::string& msg);
  
  private:
    Owner* d_owner = nullptr;
  
    std::unique_ptr<CORE::FileParamManager> d_file_param_man;
  
    std::string d_ssid;
    std::string d_pw;

    bool d_has_error = false;

    KERN::AsioCallbackTimer d_zero_timer = KERN::AsioCallbackTimer("P_WIFI::Configurator:: Zero timer");
  };
};
#endif
