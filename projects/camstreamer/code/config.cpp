#include "config.h"

#include <sstream>
#include <fstream>

//----------------------------------------------------------------------//
Config::Config(Owner* o,
	       const std::string file_path)
  : d_owner(o),
    d_extractor(new CORE::FileParamExtractor(std::move(file_path)))
{
  assert(o != nullptr);
}

//----------------------------------------------------------------------//
void Config::parseFile()
{
  if (!extractWifiSSID())
    {
      return;
    }
  
  if (!extractWifiPw())
    {
      return;
    }
  
  // Note: these pins cannot be the shutdown pin (this pin is still to be selected)

  if (!extractPinNumber(d_mode_pin,
			"mode_pin_num=",
			Error::ModePinNum))
    {
      return;
    }
  
  if (!extractPinPullMode(d_mode_pin,
			  "mode_pin_pull_mode=",
			  Error::ModePinPullMode))
    {
      return;
    }
  
  if (!extractPinNumber(d_trigger_pin,
			"trigger_pin_num=",
			Error::TriggerPinNum))
    {
      return;
    }
  
  if (!extractPinPullMode(d_trigger_pin,
			  "trigger_pin_pull_mode=",
			  Error::TriggerPinPullMode))
    {
      return;
    }

  if (!extractPinNumber(d_connect_pin,
			"connect_pin_num=",
			Error::ConnectPinNum))
    {
      return;
    }
  
  if (!extractPinPullMode(d_connect_pin,
			  "connect_pin_pull_mode=",
			  Error::ConnectPinPullMode))
    {
      return;
    }
}

//----------------------------------------------------------------------//
void Config::ownerHandleError(Error e, const std::string& msg)
{
  d_error = e;
  std::string final_msg = "Config::";
  final_msg += msg;
  if (d_owner != nullptr)
    {
      d_owner->handleError(this,e,final_msg);
    }
}

//----------------------------------------------------------------------//
bool Config::extractWifiSSID()
{
  // is empty on a failure
  d_wifi_ssid = d_extractor->getParam("wifi_ssid=");

  if (d_extractor->hasError())
    {
      std::ostringstream stream("extractWifiSSID - Failed to extract the wifi ssid with the file parameter extractor, which has the following error:", std::ios_base::app);
      stream << "\n" << d_extractor->getErrorMsg();
      ownerHandleError(Error::WifiSSID, stream.str());
      return false;
    }
  
  return true;
}

//----------------------------------------------------------------------//
bool Config::extractWifiPw()
{
  // is empty on a failure
  d_wifi_pw = d_extractor->getParam("wifi_pw=");

  if (d_extractor->hasError())
    {
      std::ostringstream stream("extractWifiSSID - Failed to extract the wifi password with the file parameter extractor, which has the following error:", std::ios_base::app);
      stream << "\n" << d_extractor->getErrorMsg();
      ownerHandleError(Error::WifiSSID, stream.str());
      return false;
    }
  
  return true;
}

//----------------------------------------------------------------------//
bool Config::extractPinPullMode(PinConfig& pin_conf,
				const std::string& pin_mode_match,
				Error e)
{
  // is empty on a failure
  std::string mode_text = d_extractor->getParam(pin_mode_match);

  if (d_extractor->hasError())
    {
      std::ostringstream stream("extractPinPullMode - Failed to extract the parameter value of '",std::ios_base::app);
      stream << pin_mode_match << "' from the file '"
	     <<  d_extractor->getFilePath() << "' with the file parameter extractor; which has the following error:"
	     << "\n" << d_extractor->getErrorMsg();
      ownerHandleError(e, stream.str());
      return false;
    }

  if (mode_text == "up")
    {
      pin_conf.pull_mode = P_WP::PullMode::Up;
      return true;
    }

  if (mode_text == "none")
    {
      pin_conf.pull_mode = P_WP::PullMode::None;
      return true;
    }

  if (mode_text == "down")
    {
      pin_conf.pull_mode = P_WP::PullMode::Down;
      return true;
    }
  
  std::ostringstream stream("extractPinPullMode - Failed to extract the parameter value of '",std::ios_base::app);
  stream << pin_mode_match << "' from the file '"
	 <<  d_extractor->getFilePath() << "'.";
  ownerHandleError(e, stream.str());
  return false;
}

//----------------------------------------------------------------------//
bool Config::extractPinNumber(PinConfig& pin_conf,
			      const std::string& pin_num_match,
			      Error e)
{
  // is empty on a failure
  std::string num_text = d_extractor->getParam(pin_num_match);

  if (d_extractor->hasError())
    {
      std::ostringstream stream("extractPinNumber - Failed to extract the parameter value of '",std::ios_base::app);
      stream << pin_num_match << "' from the file '"
	     <<  d_extractor->getFilePath() << "' with the file parameter extractor; which has the following error:"
	     << "\n" << d_extractor->getErrorMsg();
      ownerHandleError(e, stream.str());
      return false;
    }

  int pin_num = 0;
  try
    {
      pin_num = std::stoi(num_text);
    }
  catch (...)
    {
      std::ostringstream stream("extractPinNumber - Failed to convert text '",
				std::ios_base::app);
      stream << num_text << "' to a number for '" << pin_num_match << "'.";
      ownerHandleError(e, stream.str());
      return false;
    }

  if (pin_num < 0 || pin_num >= static_cast<int>(P_WP::PinNum::Unknown))
    {
      std::ostringstream stream("extractPinNumber - The pin number of '",
				std::ios_base::app);
      stream << num_text << "' is out of range.";
      ownerHandleError(e, stream.str());
      return false;
    }
  pin_conf.num = static_cast<P_WP::PinNum>(pin_num);
  return true;
}

//----------------------------------------------------------------------//
bool Config::hasError()
{
  return d_error != Error::None;
}

//----------------------------------------------------------------------//
const Config::PinConfig Config::getPinConfig(PinId id)
{
  switch (id)
    {
    case PinId::Mode:
      return d_mode_pin;
      
    case PinId::Trigger:
      return d_trigger_pin;
      
    case PinId::Connect:
      return d_connect_pin;
    }
  assert(false);
  std::ostringstream stream("getPinConfig - the pin ID '",
			    std::ios_base::app);
  stream << static_cast<int>(id) << "' does not exist. Failed to determine the pin number.";
  ownerHandleError(Error::PinId, stream.str());
  return PinConfig();
}

//----------------------------------------------------------------------//
P_WP::PinNum Config::getPinNum(PinId id)
{
  switch (id)
    {
    case PinId::Mode:
      return d_mode_pin.num;
      
    case PinId::Trigger:
      return d_trigger_pin.num;
      
    case PinId::Connect:
      return d_connect_pin.num;
    }
  assert(false);
  std::ostringstream stream("getPinNum - the pin ID '",
			    std::ios_base::app);
  stream << static_cast<int>(id) << "' does not exist. Failed to determine the pin number.";
  ownerHandleError(Error::PinId, stream.str());
  return P_WP::PinNum::H11;
}

//----------------------------------------------------------------------//
P_WP::PullMode Config::getPinPullMode(PinId id)
{
  switch (id)
    {
    case PinId::Mode:
      return d_mode_pin.pull_mode;
      
    case PinId::Trigger:
      return d_trigger_pin.pull_mode;
      
    case PinId::Connect:
      return d_connect_pin.pull_mode;
    }
  assert(false);
  std::ostringstream stream("getPinPullMode - the pin ID '",
			    std::ios_base::app);
  stream << static_cast<int>(id) << "' does not exist. Failed to determine pull mode.";
  ownerHandleError(Error::PinId, stream.str());
  return P_WP::PullMode::Up;
}
