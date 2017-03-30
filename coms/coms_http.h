#ifndef COMS_HTTP_H
#define COMS_HTTP_H

namespace COMS
{
  class SimpleHttpPost;
  class SimpleHttpPostOwner
  {
  public:
    virtual ~SimpleHttpPostOwner();
    
  private:
    friend SimpleHttpPost;
  };
  
  class SimpleHttpPost final
  {
  public:
    //do not include the post parameters
    explicit SimpleHttpPost(std::string url); //example url = "http://postit.example.com/moo.cgi"
    ~SimpleHttpPost();
    SimpleHttpPost& operator=(const SimpleHttpPost&) = delete;
    SimpleHttpPost(const SimpleHttpPost&) = delete;

    CURLcode post(std::string params); //example params = "name=daniel&project=curl"
    
  private:
    std::string d_url;
    CURL *d_curl = nullptr;
  };
};

#endif
