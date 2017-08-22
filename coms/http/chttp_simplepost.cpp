#include "chttp_simplepost.h"

using namespace C_HTTP;

//----------------------------------------------------------------------//
SimpleHttp::~SimpleHttp()
{
  if (d_curl != nullptr)
    {
      curl_easy_cleanup(d_curl);
    }
  curl_global_cleanup();
}

//----------------------------------------------------------------------//
bool SimpleHttp::init(long timeout_sec)
{
  if (isReady())
    {
      return true;
    }
  
  if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
    {
      return false;
    }

  if (d_curl != nullptr)
    {
      curl_easy_cleanup(d_curl);
    }
  
  d_curl = curl_easy_init();
  if (d_curl == nullptr)
    {
      return false;
    }

  if (curl_easy_setopt(d_curl, CURLOPT_TIMEOUT, timeout_sec) != CURLE_OK)
    {
      return false;
    }
  
  d_ready = true;
  return true;
}

//----------------------------------------------------------------------//
bool SimpleHttp::post(const std::string& url)
{
  curl_easy_setopt(d_curl, CURLOPT_URL, url.c_str());
  
  d_res = curl_easy_perform(d_curl);
  if (d_res != CURLE_OK)
    {
      return false;
    }
  return true;
}

//----------------------------------------------------------------------//
std::string SimpleHttp::getError()
{
  return std::string(curl_easy_strerror(d_res));
}
