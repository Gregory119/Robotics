#include "chttp_simplepost.h"

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

  bool res = true;
  if (argc < 3)
    {
      res = poster.postWithParams("");
    }
  else
    {
      res = poster.postWithParams(argv[2]);
    }

  if (!res)
    {
      std::cout << "Error: " << poster.getPostError() << std::end;
    }
}
