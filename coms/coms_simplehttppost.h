#ifndef COMS_SIMPLEHTTPPOST_H
#define COMS_SIMPLEHTTPPOST_H

#include <curl/curl.h>
#include <string>

namespace COMS
{
  class SimpleHttpPost final
  {
  public:
    //do not include the post parameters in the url
    explicit SimpleHttpPost(std::string url); //example url = "http://postit.example.com/moo.cgi"
    ~SimpleHttpPost();
    SimpleHttpPost& operator=(const SimpleHttpPost&) = delete;
    SimpleHttpPost(const SimpleHttpPost&) = delete;

    void setUrlNoParams(const std::string& url) { d_url = url; }
    CURLcode postWithParams(const std::string& params); //example params = "name=daniel&project=curl"
    
  private:
    std::string d_url;
    CURL *d_curl = nullptr;
    std::string res_desc;
  };
};

#endif
