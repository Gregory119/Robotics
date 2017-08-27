#include "chttp_simple.h"

#include "kn_stdkernel.h"

#include <cassert>
#include <iostream>

class TestHttp : C_HTTP::SimpleHttpOwner
{
public:
  TestHttp(const std::string& url)
  {
    d_http.reset(new C_HTTP::SimpleHttp(static_cast<C_HTTP::SimpleHttpOwner*>(this)));

    if (!d_http->init(5))
      {
	std::cout << "Failed to init http." << std::endl;
      }

    d_http->get(url);
  }
	
private:
  // SimpleHttpOwner
  void handleFailed(C_HTTP::SimpleError err) override
  {
    switch (err)
      {
      case C_HTTP::SimpleError::Internal:
	std::cout << "Got an internal error." << std::endl;
	return;

      case C_HTTP::SimpleError::Timeout:
	std::cout << "Got a timeout error." << std::endl;
	return;
      }
    assert(false);
  }
	
  void handleResponse(C_HTTP::HttpResponseCode code,
		      const std::vector<std::string>& headers,
		      const std::vector<char>& body) override
  {
    std::cout << "Received response\n";
    std::cout << "Response code is " << code << "\n\n";

    std::cout << "Response headers are:\n";
    for (const auto& header : headers)
      {
	std::cout << header << "\n";
      }
    std::cout << "\n";

    std::cout << "Response body is:\n";
    for (const auto& byte : body)
      {
	std::cout << byte;
      }
    std::cout << std::endl;
  }

private:
  std::unique_ptr<C_HTTP::SimpleHttp> d_http;
};

int main(int argc, char* argv[])
{
  using namespace C_HTTP;
  
  if (argc < 1)
    {
      std::cout << "parameters: [url]" << std::endl;
      return 0;
    }

  KERN::StdKernel k;
  TestHttp http_get_test(argv[1]);
	
  return k.run();
}
