#ifndef WIFICONFIGURATOR_H
#define WIFICONFIGURATOR_H

#include "core_fileparamextractor.h"
#include "core_owner.h"

#include <memory>
#include <string>

// Expected file format:
// - the parsing method allows for the the ssid and psk to be in any order
// -------------------------------------//
// country=GB
// ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
// update_config=1

// network={
//          ssid="<ssid>"
//          psk="<password>"
// }
// -------------------------------------//

class WifiConfigurator final
{
 public:
  enum class Error
  {
    SSID,
      Password
  };

 public:
  class Owner
  {
    OWNER_SPECIAL_MEMBERS(WifiConfigurator);
    virtual void handleError(WifiConfigurator*, Error, const std::string& msg) = 0;
  };
  
 public:
  WifiConfigurator(Owner*, std::string file_path); // uses the systemd wpa_supplicant.conf file

  void parseFile(); // parses and extracts current wifi settings
  const std::string& getSSID() { return d_ssid; }
  const std::string& getPassword() { return d_pw; }
  void setSSID(const std::string&);
  void setPassword(const std::string&);
  
 private:
  void ownerHandleError(Error, const std::string& msg);
  bool extractSSID();
  bool extractPassword();
  
 private:
  Owner* d_owner = nullptr;
  
  std::unique_ptr<CORE::FileParamExtractor> d_extractor;
  
  std::string d_ssid;
  std::string d_pw;
};

#endif
