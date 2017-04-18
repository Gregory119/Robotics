#include "chttp_simplepost.h"

#include <iostream>

int main(int argc, char* argv[])
{
  using namespace C_HTTP;
  
  if (argc < 1)
    {
      std::cout << "parameters: [url]" << std::endl;
      return 0;
    }

  SimpleHttpPost poster;

  bool res = true;
  if (poster.isReady())
    {
      res = poster.post(argv[1]);
    }

  if (!res)
    {
      std::cout << "Error: " << poster.getPostError() << std::endl;
    }
}
