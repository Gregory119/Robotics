#include "utl_mapping.h"
#include <iostream>

int main(int argc, char* argv[])
{
  using namespace UTIL;

  UTIL::Map mapper(180,
		   0,
		   2000,
		   0);
  int in = 58;
  std::cout << "input number is " << in << std::endl;
  std::cout << "mapped number is " << mapper.map<int>(in) << std::endl;
				 
  return 0;
}
