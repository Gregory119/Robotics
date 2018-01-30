#include "config.h"

#include <sstream>
#include <fstream>

//----------------------------------------------------------------------//
Config::Config(Owner* o,
	       const std::string file_path)
  : d_owner(o),
    d_file_path(std::move(file_path))
{
  assert(o != nullptr);
}

//----------------------------------------------------------------------//
void Config::parseFile()
{
  // read, parse, and validify textfile
  std::ifstream file(d_file_path);

  if (file.fail())
    {
      assert(false);
      ownerHandleError(Error::OpenFile, "Config - Failed to open the file.");
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
void Config::ownerHandleError(Error e, const std::string& msg)
{
  d_error = e;
  if (d_owner != nullptr)
    {
      d_owner->handleError(this, d_error, msg);
    }
}

//----------------------------------------------------------------------//
bool Config::extractPinPullMode(PinConfig& pin_conf,
				std::ifstream& file,
				const std::string& pin_mode_text,
				Error e)
{
  std::string line;
  std::getline(file, line);
  if (line.find(pin_mode_text) != 0)
    {
      std::ostringstream stream("Config - Failed to find '",
				std::ios_base::app);
      stream << pin_mode_text << "'.";
      ownerHandleError(e, stream.str());
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

  std::ostringstream stream("Config - Failed to interpret the pin pull mode of '",
			    std::ios_base::app);
  stream << line << "'.";
  ownerHandleError(e, stream.str());
  return false;
}

//----------------------------------------------------------------------//
bool Config::extractPinNumber(PinConfig& pin_conf,
			      std::ifstream& file,
			      const std::string& pin_num_text,
			      Error e)
{
  std::string line;
  std::getline(file, line);
  if (line.find(pin_num_text) != 0)
    {
      std::ostringstream stream("Config - Failed to find '",
				std::ios_base::app);
      stream << pin_num_text << "'.";
      ownerHandleError(e, stream.str());
      return false;
    }

  int pin_num = 0;
  std::string text_num;
  try
    {
      text_num = line.substr(pin_num_text.length());
      pin_num = std::stoi(text_num);
    }
  catch (...)
    {
      std::ostringstream stream("Config - Failed to convert text '",
				std::ios_base::app);
      stream << text_num << "'" << "to a number for '" << pin_num_text << "'.";
      ownerHandleError(e, stream.str());
      return false;
    }

  if (pin_num < 0 || pin_num >= static_cast<int>(P_WP::PinNum::Unknown))
    {
      std::ostringstream stream("Config - The pin number of '",
				std::ios_base::app);
      stream << "' is out of range.";
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
  std::ostringstream stream("Config - the pin ID '",
			    std::ios_base::app);
  stream << static_cast<int>(id) << "does not exist. Failed to determine the pin number.";
  ownerHandleError(Error::PinIdForPinNum, stream.str());
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
  std::ostringstream stream("Config - the pin ID '",
			    std::ios_base::app);
  stream << static_cast<int>(id) << "does not exist. Failed to determine pull mode.";
  ownerHandleError(Error::PinIdForPullMode, stream.str());
  return P_WP::PullMode::Up;
}
