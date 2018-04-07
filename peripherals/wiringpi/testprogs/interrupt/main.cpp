/*
  This test program reads pwm values from a specified pin number and prints them out.
 */

#include "wp_interrupt.h"

#include "kn_asiokernel.h"
#include <wiringPi.h>

#include <iostream>

class Test final : P_WP::Interrupt::Owner
{
 public:
  Test(P_WP::PinNum pin)
    : d_interrupt(new P_WP::Interrupt(this,
				      pin,
				      P_WP::Interrupt::EdgeMode::Both))
    {
      d_interrupt->start();
    }
  
 private:
  void handleInterrupt(P_WP::Interrupt*, P_WP::Interrupt::Vals&& vals) override
  {
    std::cout << "Int state = " << vals.pin_state << std::endl;
  }
  
  void handleError(P_WP::Interrupt*, P_WP::Interrupt::Error, const std::string&) override
  {
    std::cout << "handleError(Interrupt*...)" << std::endl;
  }

 private:
  std::unique_ptr<P_WP::Interrupt> d_interrupt;
};

//----------------------------------------------------------------------//
void PrintHelp()
{
  std::cout << "./interrupt [pin]\n" 
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
