#include "coms_http.h"

#include <curl/curl.h>

//----------------------------------------------------------------------//
SimpleHttpPost::SimpleHttpPost(std::string url)
  : d_url(std::move(url))
{
  curl_global_init(CURL_GLOBAL_ALL);
  d_curl = curl_easy_init();
  
  if (d_curl != nullptr)
    {
      curl_easy_setopt(curl, CURLOPT_URL, d_url.c_str());
    }
}

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
CURLcode SimpleHttpPost::post(std::string params)
{
  CURLcode ret = ;
  if (d_curl != nullptr)
    {
      curl_easy_setopt(d_curl, CURLOPT_POSTFIELDS, params.c_str());
      ret = curl_easy_perform(d_curl);
      if (ret != CURLE_OK)
	{
	  
	}
    }
  return ret;
}
