#include "chttp_simplehttppost.h"
#include <assert.h>

using namespace C_HTTP;

//----------------------------------------------------------------------//
SimpleHttpPost::SimpleHttpPost(std::string url)
  : d_url(std::move(url))
{  
  curl_global_init(CURL_GLOBAL_ALL);
  d_curl = curl_easy_init();
  setUrlNoParams(url);
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
void SimpleHttpPost::setUrlNoParams(const std::string& url)
{
  if (url.empty())
    {
      return;
    }
  
  d_url = url;
  if (d_curl != nullptr)
    {
      curl_easy_cleanup(d_curl);
    }
  curl_easy_setopt(d_curl, CURLOPT_URL, d_url.c_str());
}

//----------------------------------------------------------------------//
CURLcode SimpleHttpPost::postWithParams(const std::string& params)
{
  assert(d_curl != nullptr);

  if (!params.empty())
    {
      curl_easy_setopt(d_curl, CURLOPT_POSTFIELDS, params.c_str());
    }
  return curl_easy_perform(d_curl);
}
