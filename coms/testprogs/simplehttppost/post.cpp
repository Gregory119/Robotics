#include "coms_simplehttppost.h"

#include <iostream>

int main(int argc, char* argv[])
{
  using namespace COMS;
  
  if (argc < 2)
    {
      std::cout << "parameters: [url with no params] [params urn]" << std::endl;
      return 0;
    }

  COMS::SimpleHttpPost poster(argv[1]);
  
  if (argc < 3)
    {
      poster.postWithParams("");
    }
  else
    {
      poster.postWithParams(argv[2]);
    }
}
