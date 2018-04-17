#include "wifi_configurator.h"

#include <cassert>
#include <sstream>

using namespace P_WIFI;

static const std::string s_ssid_match = "ssid=";
static const std::string s_pw_match = "psk=";

//----------------------------------------------------------------------//
Configurator::Configurator(Owner* o, std::string file_path)
  : d_owner(o),
    d_file_param_man(new CORE::FileParamManager(std::move(file_path)))
{
  assert(o != nullptr);
}

//----------------------------------------------------------------------//
void Configurator::parseFile()
{
  d_ssid.erase();
  d_pw.erase();
  
  extractSSID();
  extractPassword();
}

//----------------------------------------------------------------------//
void Configurator::setSsid(const std::string& new_ssid)
{
  d_file_param_man->setParamFromTo(s_ssid_match, new_ssid);
  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("setSSID - Failed to set the SSID because of the following error: \n",
				std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      ownerHandleError(Error::SSID, stream.str());
      return;
    }
}

//----------------------------------------------------------------------//
void Configurator::setSsidWithQuotes(const std::string& ssid)
{
  std::string new_ssid = "\"";
  new_ssid += ssid;
  new_ssid += "\"";
  setSsid(new_ssid);
}

//----------------------------------------------------------------------//
void Configurator::setPasswordWithQuotes(const std::string& pw)
{
  std::string new_pw = "\"";
  new_pw += pw;
  new_pw += "\"";
  setPassword(new_pw);
}

//----------------------------------------------------------------------//
void Configurator::setPassword(const std::string& new_pw)
{
  d_file_param_man->setParamFromTo(s_pw_match, new_pw);
  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("setPassword - Failed to set the Password because of the following error: \n",
				std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      ownerHandleError(Error::Password, stream.str());
      return;
    }
}

//----------------------------------------------------------------------//
void Configurator::extractSSID()
{
  d_ssid.erase();
  Error e = Error::SSID;

  std::string ssid_text = d_file_param_man->getParam(s_ssid_match);
  // eg. ssid_text = "HUAWEI-E5330-469A"
  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractSSID - Failed to extract the SSID because of the following error: \n", std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      ownerHandleError(e, stream.str());
      return;
    }

  if (!d_file_param_man->removeQuotesFromTo(ssid_text,d_ssid))
    {
      std::ostringstream stream("extractSSID - The SSID parameter value '",
				std::ios_base::app);
      stream << ssid_text << "' is not in the correct format in the file '"
	     << d_file_param_man->getFilePath() << "'.";
      ownerHandleError(e,stream.str());
      return;
    }
}

//----------------------------------------------------------------------//
void Configurator::extractPassword()
{
  d_pw.erase();
  Error e = Error::Password;
  
  std::string pw_text = d_file_param_man->getParam(s_pw_match);
  // eg. psk="fta43ea9"
  if (d_file_param_man->hasError())
    {
      std::ostringstream stream("extractPassword - Failed to extract the password because of the following error: \n",
				std::ios_base::app);
      stream << d_file_param_man->getErrorMsg();
      ownerHandleError(e, stream.str());
      return;
    }
  
  if (!d_file_param_man->removeQuotesFromTo(pw_text,d_pw))
    {
      std::ostringstream stream("extractPassword - The password parameter value '",
				std::ios_base::app);
      stream << pw_text << "' is not in the correct format in the file '"
	     << d_file_param_man->getFilePath() << "'.";
      ownerHandleError(e,stream.str());
      return;
    }
}
  
//----------------------------------------------------------------------//
void Configurator::ownerHandleError(Error e, const std::string& msg)
{
  d_has_error=true;
  std::string final_msg = "P_WIFI::Configurator::";
  final_msg += msg;
  d_owner.call(&Owner::handleError,this,e,final_msg);
}
