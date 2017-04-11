#include "chttp_simplepost.h"

using namespace C_HTTP;

//----------------------------------------------------------------------//
SimpleHttpPost::~SimpleHttpPost()
{
  if (d_curl != nullptr)
    {
      curl_easy_cleanup(d_curl);
    }
  curl_global_cleanup();
}

//----------------------------------------------------------------------//
bool SimpleHttpPost::init()
{
  if (isReady())
    {
      return true;
    }
  
  if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK)
    {
      return false;
    }
  
  d_curl = curl_easy_init();
  if (d_curl == nullptr)
    {
      return false;
    }

  d_ready = true;
  return true;
}

//----------------------------------------------------------------------//
bool SimpleHttpPost::post(const std::string& url)
{
  curl_easy_setopt(d_curl, CURLOPT_URL, url.c_str());
  
  d_post_res = curl_easy_perform(d_curl);
  if (d_post_res != CURLE_OK)
    {
      return false;
    }
  return true;
}

//----------------------------------------------------------------------//
std::string SimpleHttpPost::getPostError()
{
  return std::string(curl_easy_strerror(d_post_res));
}
