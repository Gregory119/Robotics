#include "config.h"

#include <fstream>

//----------------------------------------------------------------------//
Config::Config(Owner* o,
	       const std::string& file_path)
  : d_owner(o)
{
  d_zero_fail_timer.setTimeoutCallback([&](){
      d_owner->handleConfigError(this, d_error, d_error_msg);
    });
  
  // read, parse, and validify textfile
  ifstream file(file_path.c_str());

  if (file.fail())
    {
      assert(false);
      singleShotError(Error::OpenFile, "Config - Failed to open the file.");
      return;
    }

  // Expected file format:
  // -------------------------------------//
  // mode_pin_num=<number>
  // mode_pin_pull_mode=<up/down/none>
  // trigger_pin_num=<number>
  // trigger_pin_pull_mode=<up/down/none>
  // connect_pin_num=<number>
  // connect_pin_pull_mode=<up/down/none>
  // EOF
  // -------------------------------------//
  
  // Note: these pins cannot be the shutdown pin (this pin is still to be selected)

  if (!extractPinNumber(d_mode_pin,
			file,
			"mode_pin_num=",
			Error::ModePinNum))
    {
      return;
    }
  
  if (!extractPinPullMode(d_mode_pin,
			  file,
			  "mode_pin_pull_mode=",
			  Error::ModePinPullMode))
    {
      return;
    }
  
  if (!extractPinNumber(d_trigger_pin,
			file,
			"trigger_pin_num=",
			Error::TriggerPinNum))
    {
      return;
    }
  
  if (!extractPinPullMode(d_trigger_pin,
			  file,
			  "trigger_pin_pull_mode=",
			  Error::TriggerPinPullMode))
    {
      return;
    }

  if (!extractPinNumber(d_connect_pin,
			file,
			"connect_pin_num=",
			Error::ConnectPinNum))
    {
      return;
    }
  
  if (!extractPinPullMode(d_connect_pin,
			  file,
			  "connect_pin_pull_mode=",
			  Error::ConnectPinPullMode))
    {
      return;
    }
}

//----------------------------------------------------------------------//
void Config::singleShotError(Error e, const std::string& msg)
{
  d_error = e;
  d_error_msg = msg;
  d_zero_fail_timer.singleShot(std::chrono::milliseconds(0));
}

//----------------------------------------------------------------------//
void Config::extractPinPullMode(PinConfig& pin_conf,
				ifstream& file,
				const std::string& pin_mode_text,
				Error e)
{
  std::string line;
  std::getline(file, line);
  if (line.find(pin_mode_text) != 0)
    {
      singleShotError(e,
		      "Config - Failed to find '%s'.",
		      pin_mode_text.c_str());
      return false;
    }

  std::string text_mode;
  text_mode = line.substr(pin_mode_text.length());
  
  if (text_mode == "up")
    {
      pin_conf.pull_mode = P_WP::PullMode::Up;
      return true;
    }

  if (text_mode == "none")
    {
      pin_conf.pull_mode = P_WP::PullMode::None;
      return true;
    }

  if (text_mode == "down")
    {
      pin_conf.pull_mode = P_WP::PullMode::Down;
      return true;
    }

  singleShotError(e,
		  "Config - Failed to interpret the pin pull mode of '%s'.",
		  line.c_str());
  return false;
}

//----------------------------------------------------------------------//
void Config::extractPinNumber(PinConfig& pin_conf,
			      ifstream& file,
			      const std::string& pin_num_text,
			      Error e)
{
  std::string line;
  std::getline(file, line);
  if (line.find(pin_num_text) != 0)
    {
      singleShotError(e,
		      "Config - Failed to find '%s'.",
		      pin_num_text.c_str());
      return false;
    }

  int pin_num = 0;
  std::string text_num;
  try
    {
      text_num = line.substr(pin_num_text.length());
      pin_num = std::stoi(text_num);
    }
  catch
    {
      singleShotError(e,
		      "Config - Failed to convert text '%s' to a number for '%s'.",
		      text_num.c_str(),
		      pin_num_text.c_str());
      return false;
    }

  if (pin_num < 0 || pin_num >= static_cast<int>(P_WP::PinNum::Unknown))
    {
      singleShotError(e, "Config - The pin number of '%s' is out of range.", line.c_str());
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
}

//----------------------------------------------------------------------//
P_WP::PinNum Config::getPinPullMode(PinId id)
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
}
