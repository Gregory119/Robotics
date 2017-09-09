#include "chttp_operations.h"

#include "kn_stdkernel.h"

#include <cassert>
#include <iostream>

class TestHttp : C_HTTP::HttpOperationsOwner
{
public:
  TestHttp(const std::string& url)
  {
    d_http.reset(new C_HTTP::HttpOperations(static_cast<C_HTTP::HttpOperationsOwner*>(this)));

    if (!d_http->init(60))
      {
	std::cout << "Failed to init http." << std::endl;
      }

    d_http->get(url);
  }
	
private:
  // HttpOperationsOwner
  void handleFailed(C_HTTP::HttpOperations*,C_HTTP::HttpOpError err) override
  {
    switch (err)
      {
      case C_HTTP::HttpOpError::Internal:
	std::cout << "Got an internal error." << std::endl;
	return;

      case C_HTTP::HttpOpError::Timeout:
	std::cout << "Got a timeout error." << std::endl;
	return;
      }
    assert(false);
  }
	
  void handleResponse(C_HTTP::HttpOperations*,
		      C_HTTP::HttpResponseCode code,
		      const std::vector<std::string>& headers,
		      const std::vector<char>& body) override
  {
    std::cout << "Received response\n";
    std::cout << "Response code: " << code << "\n\n";

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
  std::unique_ptr<C_HTTP::HttpOperations> d_http;
};

int main(int argc, char* argv[])
{
  using namespace C_HTTP;
  
  if (argc < 2)
    {
      std::cout << "parameters: [url]" << std::endl;
      return 0;
    }

  KERN::StdKernel k;
  TestHttp http_get_test(argv[1]);
	
  k.run();
  return 0;
}
