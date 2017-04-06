#include "chttp_simplehttppost.h"

#include <iostream>

int main(int argc, char* argv[])
{
  using namespace C_HTTP;
  
  if (argc < 2)
    {
      std::cout << "parameters: [url with no params] [params urn]" << std::endl;
      return 0;
    }

  SimpleHttpPost poster(argv[1]);
  
  if (argc < 3)
    {
      poster.postWithParams("");
    }
  else
    {
      poster.postWithParams(argv[2]);
    }
}
