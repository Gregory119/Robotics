#ifndef CHTTP_SIMPLEHTTPPOST_H
#define CHTTP_SIMPLEHTTPPOST_H

#include <curl/curl.h>
#include <string>

/*
  SimpleHttpPost is based on synchronous http communication.
 */

namespace C_HTTP
{
  class SimpleHttpPost final
  {
  public:
    SimpleHttpPost() = default;
    ~SimpleHttpPost();
    SimpleHttpPost& operator=(const SimpleHttpPost&) = delete;
    SimpleHttpPost(const SimpleHttpPost&) = delete;

    bool init(long timeout_sec = 2); //must be successful before using class
    bool isReady() { return d_ready; }
    bool post(const std::string& url); //example params = "name=daniel&project=curl"
    std::string getPostError();
    
  private:
    CURL *d_curl = nullptr;
    CURLcode d_post_res = CURLE_OK;
    bool d_ready = false;
  };
};

#endif
