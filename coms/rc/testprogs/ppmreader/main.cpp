/*
  This test program reads pwm values from a specified pin number and prints them out.
 */

#include "crc_ppmreader.h"

#include "kn_asiokernel.h"
#include <wiringPi.h>

#include <iostream>

class Test final : C_RC::PpmReader<unsigned>::Observer
{
 public:
  Test(P_WP::PinNum pin)
    : d_ppmreader(new C_RC::PpmReader<unsigned>(pin,
						C_RC::PwmLimits<unsigned>::create(C_RC::PwmLimitsType::Narrow,2000,1000)))
    {
      d_ppmreader->capData();
      d_ppmreader->attachObserver(this);
      d_ppmreader->start();
    }
  
 private:
  // PpmReader::Observer
  void handleValue(C_RC::PpmReader<unsigned>*, unsigned val) override
  {
    std::cout << "PwmReader::handleValue - Measured pwm output value = " << val << std::endl;
  }
  
  void handleValueOutOfRange(C_RC::PpmReader<unsigned>*,
			     unsigned val) override
  {
    std::cout << "PwmReader::handleValueOutOfRange - Measured pwm output value = " << val << std::endl;
  }

  void handleError(C_RC::PpmReader<unsigned>*,
		   C_RC::PpmReaderError,
		   const std::string& msg) override
  {
    std::cout << "PpmReader::handleError: \n" << msg << std::endl;
  }

  unsigned getChannel() const override { return 1; }

 private:
  std::unique_ptr<C_RC::PpmReader<unsigned>> d_ppmreader;
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
