#include "config.h"

#include "crc_ppmreader.h"

#include <chrono>
#include <sstream>
#include <fstream>

static const std::string s_req_mode_up = "up";
static const std::string s_req_mode_down = "down";
static const std::string s_req_mode_float = "float";
static const std::string s_req_mode_pwm = "pwm";
static const std::string s_req_mode_ppm = "ppm";

//----------------------------------------------------------------------//
bool Config::ReqConfig::isValid()
{
  if (num == -1 || mode == ReqMode::Unknown)
    {
      err_msg = "ReqConfig::isValid - The number and/or mode have not been set.";
      return false;
    }

  switch (mode)
    {
    case ReqMode::Up:
    case ReqMode::Down:
    case ReqMode::Float:
      if (!P_WP::PIN_UTILS::isNumInPinRange(num))
	{
	  err_msg = "ReqConfig::isValid - The request mode is a digital pin type, but the request number is not a valid pin number.";
	  return false;
	}
      return true;
      
    case ReqMode::Pwm:
      if (!P_WP::PIN_UTILS::isNumInPinRange(num))
	{
	  err_msg = "ReqConfig::isValid - The request mode is a PWM type, but the request number is not a valid pin number.";
	  return false;
	}
      return true;
      
    case ReqMode::Ppm:
      {
	unsigned chan = static_cast<unsigned>(num);
	if (chan > C_RC::getMaxPpmChannels() ||
	    chan == 0)
	  {
	    std::ostringstream stream("ReqConfig::isValid - The request mode is a PPM type, but the request number is not a valid channel number because it is greater than ",
				      std::ios_base::app);
	    stream << C_RC::getMaxPpmChannels() << " or equal to zero.";
	    err_msg = stream.str();
	    return false;
	  }
      }
      return true;
      
    case ReqMode::Unknown:
      // already handled above
      return false;
    }
  assert(false);
  return false;
}

//----------------------------------------------------------------------//
Config::Config(Owner* o,
	       std::string file_path)
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
      d_zero_timer.singleShotZero([=](){
	  ownerHandleError(Error::OpenFile, err_msg);
	});
    }
}

//----------------------------------------------------------------------//
void Config::parseFile()
{
  if (hasError())
    {
      assert(false);
      return;
    }
  
  extractWifiSSID();
  extractWifiPw();
  
  // allow mode values not to exist to imply that the next mode and trigger requests will flow through the trigger request
  if (!d_file_param_man->getParam("next_mode_req_num=").empty() ||
      !d_file_param_man->getParam("next_mode_req_mode=").empty()) // a next mode value has been provided
    {
      extractReqNumber("next_mode_req_num=",
		       Error::NextModeReqNum,
		       d_next_mode_req_conf);
      extractReqMode("next_mode_req_mode=",
		     Error::NextModeReqMode,
		     d_next_mode_req_conf);
      // must be valid if provided
      if (!d_next_mode_req_conf.isValid())
	{
	  std::string msg = "parseFile - Invalid Next Mode Request because of the following error:\n";
	  msg += d_next_mode_req_conf.getErrMsg();
	  ownerHandleError(Error::NextModeReqConfig, msg);
	  return;
	}
    }
    
  extractReqNumber("trigger_req_num=",
		   Error::TriggerReqNum,
		   d_trigger_req_conf);
  extractReqMode("trigger_req_mode=",
		 Error::TriggerReqMode,
		 d_trigger_req_conf);
  if (!d_trigger_req_conf.isValid())
    {
      std::string msg = "parseFile - Invalid Trigger Request because of the following error:\n";
      msg += d_trigger_req_conf.getErrMsg();
      ownerHandleError(Error::TriggerReqConfig, msg);
      return;
    }
  
  extractReqNumber("power_req_num=",
		   Error::PowerReqNum,
		   d_power_req_conf);
  extractReqMode("power_req_mode=",
		 Error::PowerReqMode,
		 d_power_req_conf);
  if (!d_power_req_conf.isValid())
    {
      std::string msg = "parseFile - Invalid Power Request because of the following error:\n";
      msg += d_power_req_conf.getErrMsg();
      ownerHandleError(Error::PowerReqConfig, msg);
      return;
    }
  
  checkDuplicateReqNums();
  extractPpmPinNum();

  extractPwmPulse("pwm_pulse_high_us=",
		  Error::PwmHigh,
		  d_pwm_high_us);
  extractPwmPulse("pwm_pulse_low_us=",
		  Error::PwmLow,
		  d_pwm_low_us);
  checkPwmPulses();
}

//----------------------------------------------------------------------//
void Config::extractWifiSSID()
{
  // is empty on a failure
  d_wifi_ssid = d_file_param_man->getParam("wifi_ssid=");

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractWifiSSID - Failed to extract the wifi ssid because of the following error: \n",
				std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      ownerHandleError(Error::WifiSSID, stream.str());
      return;
    }
}

//----------------------------------------------------------------------//
void Config::extractWifiPw()
{
  // is empty on a failure
  d_wifi_pw = d_file_param_man->getParam("wifi_pw=");

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractWifiPassword - Failed to extract the wifi password because of the following error: \n",
				std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      ownerHandleError(Error::WifiPassword, stream.str());
      return;
    }
}

//----------------------------------------------------------------------//
void Config::extractPpmPinNum()
{
  if (d_next_mode_req_conf.mode != ReqMode::Ppm &&
      d_trigger_req_conf.mode != ReqMode::Ppm &&
      d_power_req_conf.mode != ReqMode::Ppm)
    {
      return;
    }
  
  // is empty on a failure
  std::string num_text = d_file_param_man->getParam("ppm_pin_num=");

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractPpmPinNum - Failed to extract the PPM pin number because of the following error: \n",
				std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      ownerHandleError(Error::PpmPinNum, stream.str());
      return;
    }

  int ppm_pin = 0;
  try
    {
      ppm_pin = std::stoi(num_text);
    }
  catch (...)
    {
      std::ostringstream stream("extractPpmPinNum - Failed to convert text '",
				std::ios_base::app);
      stream << num_text << "' to a number.";
      ownerHandleError(Error::PpmPinNum, stream.str());
      return;
    }

  if (!P_WP::PIN_UTILS::isNumInPinRange(ppm_pin))
    {
      std::ostringstream stream("extractPpmPinNum - The PPM pin number of ",
				std::ios_base::app);
      stream << ppm_pin << " is invalid.";
      ownerHandleError(Error::PpmPinNum, stream.str());
      return;
    }

  d_ppm_pin_num = static_cast<P_WP::PinNum>(ppm_pin);
}

//----------------------------------------------------------------------//
void Config::extractReqMode(const std::string& req_mode_match,
			    Error e,
			    ReqConfig& req_conf)
{
  // is empty on a failure
  std::string mode_text = d_file_param_man->getParam(req_mode_match);

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractReqMode - Failed to extract the request mode of '",
				std::ios_base::app);
      stream << req_mode_match << "' because of the following error: \n" << d_file_param_man->getErrorMsg();
      ownerHandleError(e, stream.str());
      return;
    }

  if (mode_text == s_req_mode_pwm)
    {
      req_conf.mode = ReqMode::Pwm;
      return;
    }

  if (mode_text == s_req_mode_ppm)
    {
      req_conf.mode = ReqMode::Ppm;
      return;
    }
  
  if (mode_text == s_req_mode_up)
    {
      req_conf.mode = ReqMode::Up;
      return;
    }

  if (mode_text == s_req_mode_float)
    {
      req_conf.mode = ReqMode::Float;
      return;
    }

  if (mode_text == s_req_mode_down)
    {
      req_conf.mode = ReqMode::Down;
      return;
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
}

//----------------------------------------------------------------------//
void Config::extractReqNumber(const std::string& req_num_match,
			      Error e,
			      ReqConfig& req_conf)
{
  std::string num_text = d_file_param_man->getParam(req_num_match);

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractReqNumber - Failed to extract the request number of '",std::ios_base::app);
      stream << req_num_match <<  "' because of the following error: \n" << d_file_param_man->getErrorMsg();
      ownerHandleError(e, stream.str());
      return;
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
      return;
    }
}

//----------------------------------------------------------------------//
Config::ReqConfig Config::getReqConfig(Request req)
{
  switch (req)
    {
    case Request::NextMode:
      return d_next_mode_req_conf;
      
    case Request::Trigger:
      return d_trigger_req_conf;

    case Request::Power:
      return d_power_req_conf;
    }
  assert(false);
  std::ostringstream stream("getReqConfig - the request '",
			    std::ios_base::app);
  stream << static_cast<int>(req) << "' does not exist.";
  ownerHandleError(Error::RequestEnum, stream.str());
  return ReqConfig();
}

//----------------------------------------------------------------------//
int Config::getReqNum(Request req)
{
  switch (req)
    {
    case Request::NextMode:
      return d_next_mode_req_conf.num;
      
    case Request::Trigger:
      return d_trigger_req_conf.num;

    case Request::Power:
      return d_power_req_conf.num;
    }
  assert(false);
  std::ostringstream stream("getReqNum - the request '",
			    std::ios_base::app);
  stream << static_cast<int>(req) << "' does not exist.";
  ownerHandleError(Error::RequestEnum, stream.str());
  return -1;
}

//----------------------------------------------------------------------//
ReqMode Config::getReqMode(Request req)
{
  switch (req)
    {
    case Request::NextMode:
      return d_next_mode_req_conf.mode;
      
    case Request::Trigger:
      return d_trigger_req_conf.mode;

    case Request::Power:
      return d_power_req_conf.mode;
    }
  assert(false);
  std::ostringstream stream("getReqMode - the request '",
			    std::ios_base::app);
  stream << static_cast<int>(req) << "' does not exist.";
  ownerHandleError(Error::RequestEnum, stream.str());
  return ReqMode::Unknown;
}

//----------------------------------------------------------------------//
bool Config::getPullMode(ReqMode req_mode, P_WP::PullMode& pull_mode)
{
  switch (req_mode)
    {
    case ReqMode::Up:
      pull_mode = P_WP::PullMode::Up;
      return true;
      
    case ReqMode::Down:
      pull_mode = P_WP::PullMode::Down;
      return true;
      
    case ReqMode::Float:
      pull_mode = P_WP::PullMode::None;
      return true;
      
    case ReqMode::Pwm:
    case ReqMode::Ppm:
    case ReqMode::Unknown:
      assert(false);
      return false;
    }
  assert(false);
  return false;
}

//----------------------------------------------------------------------//
void Config::extractPwmPulse(const std::string& pulse_match,
			     Error e,
			     int& pulse_us)
{
  std::string pulse_text = d_file_param_man->getParam(pulse_match);

  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractPwmPulse - Failed to extract the pwm pulse duration of '",std::ios_base::app);
      stream << pulse_match <<  "' because of the following error: \n" << d_file_param_man->getErrorMsg();
      ownerHandleError(e, stream.str());
      return;
    }

  try
    {
      pulse_us = std::stoi(pulse_text);
    }
  catch (...)
    {
      std::ostringstream stream("extractPwmPulse - Failed to convert text '",
				std::ios_base::app);
      stream << pulse_text << "' to a number for '" << pulse_match << "'.";
      ownerHandleError(e, stream.str());
      return;
    }

  if (pulse_us < 0 || pulse_us > 3000)
    {
      std::ostringstream stream("extractPwmPulse - The pwm pulse duration of '",
				std::ios_base::app);
      stream << pulse_text << "' for '" << pulse_match << "' is out of range. It must be greater than or equal to zero and less than or equal to 3000.";
      ownerHandleError(e, stream.str());
      return;
    }
}

//----------------------------------------------------------------------//
void Config::checkDuplicateReqNums()
{
  if ((d_next_mode_req_conf.num == d_trigger_req_conf.num) &&
      areConflictingReqModes(d_next_mode_req_conf.mode, d_trigger_req_conf.mode) &&
      d_next_mode_req_conf.mode != d_trigger_req_conf.mode)
    {
      ownerHandleError(Error::CombinedControl,
		       "checkDuplicateReqNums - If the request numbers of the mode and triggers requests are the same, and their request modes are conflicting, then they must have the same request mode.");
      return;
    }

  if ((d_next_mode_req_conf.num == d_power_req_conf.num) &&
      areConflictingReqModes(d_next_mode_req_conf.mode, d_power_req_conf.mode))
    {
      ownerHandleError(Error::DuplicateReqNum,
		       "checkDuplicateReqNums - The mode and power request numbers cannot be the same, if their request modes are conflicting.");
      return;
    }

  if ((d_trigger_req_conf.num == d_power_req_conf.num) &&
      areConflictingReqModes(d_trigger_req_conf.mode, d_power_req_conf.mode))
    {
      ownerHandleError(Error::DuplicateReqNum,
		       "checkDuplicateReqNums - The trigger and power request numbers cannot be the same, if their request modes are conflicting.");
      return;
    }
}

//----------------------------------------------------------------------//
void Config::checkPwmPulses()
{
  if (d_pwm_high_us < d_pwm_low_us)
    {
      std::ostringstream stream("checkPwmPulses - Failed because the high pulse must be greater than the low pulse (low pulse [us] = ",
				std::ios_base::app);
      stream << d_pwm_low_us << ", high pulse [us] = " << d_pwm_high_us << ").";
      ownerHandleError(Error::PwmHighAndLow, stream.str());
      return;
    }
}

//----------------------------------------------------------------------//
bool Config::areConflictingReqModes(ReqMode req_l, ReqMode req_r)
{
  if ((usesPin(req_l) && usesPin(req_r)) ||
      (usesChannel(req_l) && usesChannel(req_r)))
    {
      return true;
    }
  return false;
}

//----------------------------------------------------------------------//
bool Config::usesPin(ReqMode req_mode)
{
  switch (req_mode)
    {
    case ReqMode::Up:
    case ReqMode::Down:
    case ReqMode::Float:
    case ReqMode::Pwm:
      return true;
      
    case ReqMode::Ppm:
      return false;
      
    case ReqMode::Unknown:
      assert(false);
      return false;
    }
  assert(false);
  ownerHandleError(Error::Internal, "usesPin - Failed to identify the request mode enum value.");
}

//----------------------------------------------------------------------//
bool Config::usesChannel(ReqMode req_mode)
{
  switch (req_mode)
    {
    case ReqMode::Up:
    case ReqMode::Down:
    case ReqMode::Float:
    case ReqMode::Pwm:
      return false;
      
    case ReqMode::Ppm:
      return true;
      
    case ReqMode::Unknown:
      assert(false);
      return false;
    }
  assert(false);
  ownerHandleError(Error::Internal, "usesChannel - Failed to identify the request mode enum value.");
}

//----------------------------------------------------------------------//
void Config::ownerHandleError(Error e, const std::string& msg)
{
  d_has_error = true;
  std::string final_msg = "Config::";
  final_msg += msg;
  d_owner.call(&Owner::handleError,this,e,final_msg);
}
