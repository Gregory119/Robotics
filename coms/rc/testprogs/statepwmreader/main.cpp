/*
  This test program reads pwm values from a specified pin number and prints them out.
 */

#include "crc_statepwmreader.h"
#include "kn_asiokernel.h"
#include <wiringPi.h>

#include <iostream>

class Test final : C_RC::StatePwmReader::Owner
{
 public:
  Test(P_WP::PinNum pin)
    : d_reader(new C_RC::StatePwmReader(this,
					pin,
					std::chrono::microseconds(2000),
					std::chrono::microseconds(1000)))
    {
      d_reader->start();
    }
  
 private:
  void handleState(C_RC::StatePwmReader*, bool state) override
  {
    std::cout << "StatePwmReader::handleState - state = " << state << std::endl;
  }
  
  void handleError(C_RC::StatePwmReader*,
		   C_RC::PwmReaderError,
		   const std::string& msg) override
  {
    std::cout << "StatePwmReader::handleError: \n" << msg << std::endl;
  }
  
 private:
  std::unique_ptr<C_RC::StatePwmReader> d_reader;
};

//----------------------------------------------------------------------//
void PrintHelp()
{
  std::cout << "./statepwmreader [pin]\n" 
	    << "[pin]\n" 
	    << "This is the wiring pi pin number.\n" 
	    << std::endl;
}

//----------------------------------------------------------------------//
int main(int argc, char *argv[])
{
  if (argc != 2)
    {
      PrintHelp();
      return EXIT_SUCCESS;
    }

  KERN::AsioKernel k;
  int pin;

  std::string pin_str(argv[1]);

  try
    {
      pin = std::stoi(pin_str);
    }
  catch (...)
    {
      PrintHelp();
      return EXIT_FAILURE;
    }

  if (pin < 0 || pin >= static_cast<int>(P_WP::PinNum::Unknown))
    {
      PrintHelp();
      return EXIT_FAILURE;
    }

  std::cout << "pin num = " << pin << std::endl;
  
  wiringPiSetup(); // terminates on failure
  Test test(static_cast<P_WP::PinNum>(pin));

  k.run();

  return EXIT_SUCCESS;
}
