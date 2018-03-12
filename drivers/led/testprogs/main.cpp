#include "dled_driver.h"

#include <iostream>
#include <cstdlib> //exit success/failure

void PrintHelp()
{
  std::cout << "./test_led [command] [params]\n" 
	    << "[command]\n" 
	    << "on - turn on\n" 
	    << "off - turn off\n"
	    << "flash - request to trigger\n"
	    << "  [params]\n"
	    << "  on=<on time> [ms]\n"
	    << "  off=<off time> [ms]\n"
	    << std::endl;
}

//----------------------------------------------------------------------//
int main(int argc, char *argv[])
{
  /*
  // provide led on and off timing parameters
  if (argc == 1)
    {
      PrintHelp();
      return EXIT_SUCCESS;
    }

  D_LED::Driver led_driver;
  
  if (std::string(argv[1]) == "on")
    {
      if (argc > 2)
	{
	  PrintHelp();
	  return EXIT_SUCCESS;
	}
      
      if (!led_driver.turnOn())
	{
	  return EXIT_FAILURE;
	}
    }

  if (std::string(argv[1]) == "off")
    {
      if (argc > 2)
	{
	  PrintHelp();
	  return EXIT_SUCCESS;
	}
      
      if (!led_driver.turnOff())
	{
	  return EXIT_FAILURE;
	}
    }

  if (std::string(argv[1]) == "flash")
    {
      if (argc != 4)
	{
	  PrintHelp();
	  return EXIT_SUCCESS;
	}
      
      std::string str_on(argv[2]);
      std::string str_off(argv[3]);

      try
	{
	  int ms_on = std::stoi(str_on);
	  int ms_off = std::stoi(str_off);

	  if (!led_driver.flashOnOff(std::chrono::milliseconds(ms_on),
				   std::chrono::milliseconds(ms_off)))
	    {
	      return EXIT_FAILURE;
	    }
	}
      catch (...)
	{
	  PrintHelp();
	  return EXIT_SUCCESS;
	}
    }

    PrintHelp();*/
  return EXIT_SUCCESS;
}
