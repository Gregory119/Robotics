/*
  This test program reads pwm values from a specified pin number and prints them out.
 */

#include "crc_pwmreader.h"

#include "kn_asiokernel.h"
#include <wiringPi.h>

#include <iostream>

class Test final : C_RC::PwmReader<unsigned>::Owner
{
 public:
  Test(P_WP::PinNum pin)
    : d_pwmreader(new C_RC::PwmReader<unsigned>(this,
						pin,
						C_RC::PwmLimits<unsigned>::create(C_RC::PwmLimitsType::Narrow,
										  2000,
										  0)))
    {
      d_pwmreader->capData();
      d_pwmreader->start();
    }
  
 private:
  void handleValue(C_RC::PwmReader<unsigned>*, unsigned val) override
  {
    std::cout << "PwmReader::handleValue - Measured pwm output value = " << val << std::endl;
  }
  
  void handleError(C_RC::PwmReader<unsigned>*,
		   C_RC::PwmReaderError,
		   const std::string& msg) override
  {
    std::cout << "PwmReader::handleError: \n" << msg << std::endl;
  }

  void handleValueOutOfRange(C_RC::PwmReader<unsigned>*,
			     unsigned val) override
  {
    std::cout << "PwmReader::handleValueOutOfRange - Measured pwm output value = " << val << std::endl;
  }
  
 private:
  std::unique_ptr<C_RC::PwmReader<unsigned>> d_pwmreader;
};

//----------------------------------------------------------------------//
void PrintHelp()
{
  std::cout << "./pwmreader [pin]\n" 
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
