#include "config.h"

#include <chrono>
#include <sstream>
#include <fstream>

static const std::string s_req_mode_up = "up";
static const std::string s_req_mode_down = "down";
static const std::string s_req_mode_float = "float";
static const std::string s_req_mode_pwm = "pwm";
static const std::string s_req_mode_ppm = "ppm";

//----------------------------------------------------------------------//
Config::Config(Owner* o,
	       const std::string file_path)
  : d_owner(o),
    d_file_param_man(new CORE::FileParamManager(std::move(file_path)))
{
  assert(o != nullptr);
  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("Config - Failed to start extracting configuration because of the following error: \n",
				std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      std::string err_msg = stream.str();
      d_zero_timer.setTimeoutCallback([=](){
	  ownerHandleError(Error::OpenFile, err_msg);
	});
      d_zero_timer.singleShotZero();
    }
}

//----------------------------------------------------------------------//
void Config::setOwner(Owner* o) { assert(o!=nullptr); d_owner = o; }

//----------------------------------------------------------------------//
void Config::parseFile()
{
  if (hasError())
    {
      return;
    }
  
  if (!extractWifiSSID())
    {
      return;
    }
  
  if (!extractWifiPw())
    {
      return;
    }
  
  // Note: these pins cannot be the shutdown pin (this pin is still to be selected)

  if (!extractReqNumber(d_mode_req_conf,
			"mode_req_num=",
			Error::ModeReqNum))
    {
      return;
    }
  
  if (!extractReqMode(d_mode_req_conf,
		      "mode_req_mode=",
		      Error::ModeReqMode))
    {
      return;
    }
  
  if (!extractReqNumber(d_trigger_req_conf,
			"trigger_req_num=",
			Error::TriggerReqNum))
    {
      return;
    }
  
  if (!extractReqMode(d_trigger_req_conf,
		      "trigger_req_mode=",
		      Error::TriggerReqMode))
    {
      return;
    }

  if (!extractReqNumber(d_power_req_conf,
			"power_req_num=",
			Error::PowerReqNum))
    {
      return;
    }
  
  if (!extractReqMode(d_power_req_conf,
		      "power_req_mode=",
		      Error::PowerReqMode))
    {
      return;
    }

  checkDuplicateReqNums();
}

//----------------------------------------------------------------------//
bool Config::extractWifiSSID()
{
  // is empty on a failure
  d_wifi_ssid = d_file_param_man->getParam("wifi_ssid=");

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractWifiSSID - Failed to extract the wifi ssid because of the following error: \n",
				std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      ownerHandleError(Error::WifiSSID, stream.str());
      return false;
    }
  
  return true;
}

//----------------------------------------------------------------------//
bool Config::extractWifiPw()
{
  // is empty on a failure
  d_wifi_pw = d_file_param_man->getParam("wifi_pw=");

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractWifiPassword - Failed to extract the wifi password because of the following error: \n",
				std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      ownerHandleError(Error::WifiPassword, stream.str());
      return false;
    }
  
  return true;
}

//----------------------------------------------------------------------//
bool Config::extractReqMode(ReqConfig& req_conf,
			    const std::string& req_mode_match,
			    Error e)
{
  // is empty on a failure
  std::string mode_text = d_file_param_man->getParam(req_mode_match);

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractReqMode - Failed to extract the request mode of '",
				std::ios_base::app);
      stream << req_mode_match << "' because of the following error: \n" << d_file_param_man->getErrorMsg();
      ownerHandleError(e, stream.str());
      return false;
    }

  if (mode_text == s_req_mode_pwm)
    {
      req_conf.mode = ReqMode::Pwm;
      return true;
    }

  if (mode_text == s_req_mode_ppm)
    {
      req_conf.mode = ReqMode::Ppm;
      return true;
    }
  
  if (mode_text == s_req_mode_up)
    {
      req_conf.mode = ReqMode::Up;
      return true;
    }

  if (mode_text == s_req_mode_float)
    {
      req_conf.mode = ReqMode::Float;
      return true;
    }

  if (mode_text == s_req_mode_down)
    {
      req_conf.mode = ReqMode::Down;
      return true;
    }
  
  std::ostringstream stream("extractReqMode - The request mode of '",std::ios_base::app);
  stream << req_mode_match << "' is set to an invalid value of '" << mode_text << "' in the file '"
	 <<  d_file_param_man->getFilePath() << "'. The options are '"
	 << s_req_mode_up << "', '"
	 << s_req_mode_down << "', '"
	 << s_req_mode_float << "', '"
	 << s_req_mode_pwm << "', or '"
	 << s_req_mode_ppm << ".";
  ownerHandleError(e, stream.str());
  return false;
}

//----------------------------------------------------------------------//
bool Config::extractReqNumber(ReqConfig& req_conf,
			      const std::string& req_num_match,
			      Error e)
{
  // is empty on a failure
  std::string num_text = d_file_param_man->getParam(req_num_match);

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractReqNumber - Failed to extract the request number of '",std::ios_base::app);
      stream << req_num_match <<  "' because of the following error: \n" << d_file_param_man->getErrorMsg();
      ownerHandleError(e, stream.str());
      return false;
    }

  try
    {
      req_conf.num = std::stoi(num_text);
    }
  catch (...)
    {
      std::ostringstream stream("extractReqNumber - Failed to convert text '",
				std::ios_base::app);
      stream << num_text << "' to a number for '" << req_num_match << "'.";
      ownerHandleError(e, stream.str());
      return false;
    }

  return true;
}

//----------------------------------------------------------------------//
Config::ReqConfig Config::getReqConfig(Request req)
{
  switch (req)
    {
    case Request::Mode:
      return d_mode_req_conf;
      
    case Request::Trigger:
      return d_trigger_req_conf;

    case Request::Power:
      return d_power_req_conf;
    }
  assert(false);
  std::ostringstream stream("getReqConfig - the request '",
			    std::ios_base::app);
  stream << static_cast<int>(req) << "' does not exist.";
  ownerHandleError(Error::Request, stream.str());
  return ReqConfig();
}

//----------------------------------------------------------------------//
int Config::getReqNum(Request req)
{
  switch (req)
    {
    case Request::Mode:
      return d_mode_req_conf.num;
      
    case Request::Trigger:
      return d_trigger_req_conf.num;

    case Request::Power:
      return d_power_req_conf.num;
    }
  assert(false);
  std::ostringstream stream("getReqNum - the request '",
			    std::ios_base::app);
  stream << static_cast<int>(req) << "' does not exist.";
  ownerHandleError(Error::Request, stream.str());
  return -1;
}

//----------------------------------------------------------------------//
ReqMode Config::getReqMode(Request req)
{
  switch (req)
    {
    case Request::Mode:
      return d_mode_req_conf.mode;
      
    case Request::Trigger:
      return d_trigger_req_conf.mode;

    case Request::Power:
      return d_power_req_conf.mode;
    }
  assert(false);
  std::ostringstream stream("getReqMode - the request '",
			    std::ios_base::app);
  stream << static_cast<int>(req) << "' does not exist.";
  ownerHandleError(Error::Request, stream.str());
  return ReqMode::Unknown;
}

//----------------------------------------------------------------------//
void Config::ownerHandleError(Error e, const std::string& msg)
{
  d_has_error = true;
  std::string final_msg = "Config::";
  final_msg += msg;
  if (d_owner != nullptr)
    {
      d_owner->handleError(this,e,final_msg);
    }
}

//----------------------------------------------------------------------//
void Config::checkDuplicateReqNums()
{
  if ((d_mode_req_conf.num == d_trigger_req_conf.num) &&
      (d_mode_req_conf.mode != d_trigger_req_conf.mode))
    {
      ownerHandleError(Error::CombinedControl,
		       "checkDuplicateReqNums - The mode and trigger request numbers can be the same, only if their request modes are the same, which they are currently not.");
      return;
    }

  if (d_mode_req_conf.num == d_power_req_conf.num)
    {
      ownerHandleError(Error::DuplicateReqNum,
		       "checkDuplicateReqNums - The mode and power request numbers cannot be the same.");
      return;
    }

  if (d_trigger_req_conf.num == d_power_req_conf.num)
    {
      ownerHandleError(Error::DuplicateReqNum,
		       "checkDuplicateReqNums - The trigger and power request numbers cannot be the same.");
      return;
    }
}
