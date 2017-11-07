#include "chttp_operations.h"

#include "kn_asiokernel.h"

#include <cassert>
#include <iostream>

class TestHttp : C_HTTP::OperationsOwner
{
public:
  TestHttp(const std::string& url)
  {
    d_http.reset(new C_HTTP::Operations(static_cast<C_HTTP::OperationsOwner*>(this)));

    d_http->init(60);
    d_http->get(url);
  }
	
private:
  // C_HTTP::OperationsOwner
  void handleFailed(C_HTTP::Operations*,C_HTTP::OpError err) override
  {
    switch (err)
      {
      case C_HTTP::OpError::Internal:
	std::cout << "Got an internal error. This should never happen." << std::endl;
	return;

      case C_HTTP::OpError::Timeout:
	std::cout << "Got a timeout error." << std::endl;
	return;
      }
    assert(false);
  }
	
  void handleResponse(C_HTTP::Operations*,
		      C_HTTP::ResponseCodeNum code,
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
  std::unique_ptr<C_HTTP::Operations> d_http;
};

int main(int argc, char* argv[])
{
  using namespace C_HTTP;
  
  if (argc < 2)
    {
      std::cout << "parameters: [url]" << std::endl;
      return 0;
    }

  KERN::AsioKernel k;
  TestHttp http_get_test(argv[1]);
	
  k.run();
  return 0;
}
