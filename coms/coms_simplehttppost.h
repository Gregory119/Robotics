#ifndef COMS_SIMPLEHTTPPOST_H
#define COMS_SIMPLEHTTPPOST_H

#include <curl/curl.h>
#include <string>

namespace COMS
{
  class SimpleHttpPost;
  class SimpleHttpPostOwner
  {
  public:
    virtual ~SimpleHttpPostOwner();
    SimpleHttpPostOwner& operator=(const SimpleHttpPostOwner&) = delete;
    SimpleHttpPostOwner(const SimpleHttpPostOwner&) = delete;
    
  private:
    friend SimpleHttpPost;
    virtual void handlePostReturn(CURLcode res, const std::string& desc) = 0;
  };
  
  class SimpleHttpPost final
  {
  public:
    //do not include the post parameters in the url
    explicit SimpleHttpPost(SimpleHttpPostOwner* o,
			    std::string url); //example url = "http://postit.example.com/moo.cgi"
    ~SimpleHttpPost();
    SimpleHttpPost& operator=(const SimpleHttpPost&) = delete;
    SimpleHttpPost(const SimpleHttpPost&) = delete;

    void setUrlNoParams(const std::string& url) { d_url = url; }
    void postWithParams(const std::string& params); //example params = "name=daniel&project=curl"
    
  private:
    SimpleHttpPostOwner* d_owner = nullptr;
    
    std::string d_url;
    CURL *d_curl = nullptr;
    CURLcode d_res = CURLE_OK;
    std::string res_desc;
  };
};

#endif
