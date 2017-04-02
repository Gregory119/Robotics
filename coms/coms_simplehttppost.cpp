#include "coms_simplehttppost.h"
#include <assert.h>

using namespace COMS;

//----------------------------------------------------------------------//
SimpleHttpPost::SimpleHttpPost(SimpleHttpPostOwner* o,
			       std::string url)
  : d_owner(o),
    d_url(std::move(url))
{  
  curl_global_init(CURL_GLOBAL_ALL);
  d_curl = curl_easy_init();
  
  if (d_curl != nullptr)
    {
      curl_easy_setopt(d_curl, CURLOPT_URL, d_url.c_str());
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
void SimpleHttpPost::postWithParams(const std::string& params)
{
  if (d_curl == nullptr)
    {
      return;
    }

  if (d_owner == nullptr)
    {
      return;
    }

  curl_easy_setopt(d_curl, CURLOPT_POSTFIELDS, params.c_str());
  d_res = curl_easy_perform(d_curl);
  res_desc = curl_easy_strerror(d_res);
  d_owner->handlePostReturn(d_res, res_desc);
}
