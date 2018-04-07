/*
  This test program reads pwm values from a specified pin number and prints them out.
*/

#include "crc_stateppmreader.h"

#include "kn_asiokernel.h"
#include <wiringPi.h>

#include <iostream>

class Test final : C_RC::StatePpmReader::Observer
{
 public:
  Test(P_WP::PinNum pin,
       unsigned chan)
    : d_reader(new C_RC::StatePpmReader(pin,
					std::chrono::microseconds(2000),
					std::chrono::microseconds(1000))),
    d_channel(chan)
    {
      d_reader->attachObserver(this);
      d_reader->start();
    }
  
 private:
  // StatePpmReader::Observer
  void handleState(C_RC::PpmReader<unsigned>*, bool state) override
  {
    std::cout << "StatePwmReader::handleState - state = " << state << std::endl;
  }

  void handleError(C_RC::PpmReader<unsigned>*,
		   C_RC::PpmReaderError,
		   const std::string& msg) override
  {
    std::cout << "PpmReader::handleError: \n" << msg << std::endl;
  }

  unsigned getChannel() const override { return d_channel; }
  
 private:
  std::unique_ptr<C_RC::StatePpmReader> d_reader;
  unsigned d_channel = 1;
};

//----------------------------------------------------------------------//
void PrintHelp()
{
  std::cout << "./stateppmreader [pin] [channel]\n" 
	    << "[pin]\n" 
	    << "This is the wiring pi pin number.\n"
	    << "[channel]\n"
	    << "This is the PPM channel number."
	    << std::endl;
}

//----------------------------------------------------------------------//
int main(int argc, char *argv[])
{
  if (argc != 3)
    {
      PrintHelp();
      return EXIT_SUCCESS;
    }

  KERN::AsioKernel k;
  int pin = 0;
  int chan = 0;

  std::string pin_str(argv[1]);
  std::string chan_str(argv[2]);

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

    try
    {
      chan = std::stoi(chan_str);
    }
  catch (...)
    {
      PrintHelp();
      return EXIT_FAILURE;
    }

  if (chan < 1 || chan > 16)
    {
      PrintHelp();
      return EXIT_FAILURE;
    }
  
  wiringPiSetup(); // terminates on failure
  Test test(static_cast<P_WP::PinNum>(pin),
	    chan);

  k.run();

  return EXIT_SUCCESS;
}
