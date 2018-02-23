#include "wificonfigurator.h"

#include <cassert>
#include <sstream>

//----------------------------------------------------------------------//
WifiConfigurator::WifiConfigurator(Owner* o, std::string file_path)
  : d_owner(o),
    d_extractor(new CORE::FileParamExtractor(std::move(file_path)))
{
  assert(o != nullptr);
}

//----------------------------------------------------------------------//
void WifiConfigurator::parseFile()
{
  if (!extractSSID())
    {
      return;
    }

  if (!extractPassword())
    {
      return;
    }
}

//----------------------------------------------------------------------//
void WifiConfigurator::setSSID(const std::string&)
{
  
}

//----------------------------------------------------------------------//
void WifiConfigurator::setPassword(const std::string&)
{
  
}

//----------------------------------------------------------------------//
void WifiConfigurator::ownerHandleError(Error e, const std::string& msg)
{
  std::string final_msg = "WifiConfigurator::";
  final_msg += msg;
  if (d_owner != nullptr)
    {
      d_owner->handleError(this,e,final_msg);
    }
}

//----------------------------------------------------------------------//
bool WifiConfigurator::extractSSID()
{
  d_ssid.erase();
  Error e = Error::SSID;

  std::string match = "ssid=";
  std::string ssid_text = d_extractor->getParam(match);
  // eg. ssid_text = "HUAWEI-E5330-469A"
  if (d_extractor->hasError())
    {
      std::ostringstream stream("extractSSID - Failed to extract the parameter value of '", std::ios_base::app);
      stream << match << "' from the file '"
	     <<  d_extractor->getFilePath() << "' with the file parameter extractor; which has the following error:"
	     << "\n" << d_extractor->getErrorMsg();
      ownerHandleError(e, stream.str());
      return false;
    }

  size_t start_pos = 1;
  size_t end_pos = ssid_text.find("\"",start_pos);
  if (end_pos == std::string::npos)
    {
      std::ostringstream stream("extractSSID - Failed to find the end double quote in the text '",
				std::ios_base::app);
      stream << ssid_text << "'.";
      ownerHandleError(e,stream.str());
      return false;
    }

  try
    {
      size_t num_chars = end_pos - start_pos;
      d_ssid = ssid_text.substr(start_pos, num_chars);
    }
  catch (...)
    {
      std::ostringstream stream("extractSSID - Unexpected internal exception error while extracting parameter data from '",
				std::ios_base::app);
      stream << ssid_text << "'.";
      ownerHandleError(e,stream.str());
      return false;
    }
  return true;
}

//----------------------------------------------------------------------//
bool WifiConfigurator::extractPassword()
{
  d_pw.erase();
  Error e = Error::Password;

  std::string match = "psk=";
  std::string pw_text = d_extractor->getParam("psk=");
  // eg. psk="fta43ea9"
  if (d_extractor->hasError())
    {
      std::ostringstream stream("extractPassword - Failed to extract the parameter value of '", std::ios_base::app);
      stream << match << "' from the file '"
	     <<  d_extractor->getFilePath() << "' with the file parameter extractor; which has the following error:"
	     << "\n" << d_extractor->getErrorMsg();
      ownerHandleError(e, stream.str());
      return false;
    }
  
  size_t start_pos = 1;
  size_t end_pos = pw_text.find("\"",start_pos);
  if (end_pos == std::string::npos)
    {
      std::ostringstream stream("extractPassword - Failed to find the end double quote in the text '",
				std::ios_base::app);
      stream << pw_text << "'.";
      ownerHandleError(e,stream.str());
      return false;
    }

  try
    {
      size_t num_chars = end_pos - start_pos;
      d_pw = pw_text.substr(start_pos, num_chars);
    }
  catch (...)
    {
      std::ostringstream stream("extractSSID - Unexpected internal exception error while extracting parameter data from '",
				std::ios_base::app);
      stream << pw_text << "'.";
      ownerHandleError(e,stream.str());
      return false;
    }

  return true;
}
