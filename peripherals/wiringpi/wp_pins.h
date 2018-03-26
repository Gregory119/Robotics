#ifndef PWP_PINS_H
#define PWP_PINS_H

#include <cassert>

namespace P_WP
{
  enum class PullMode
  {
    Up,
      Down,
      None
      };
  
  // The pin number comments below show the wiring pi pin number and corresponding header pin of the 20x2 headers.
  // The following raspberry pis use a 20x2 pin header:
  // Pi 1A+, 1B+, 2, 3, Zero, Zero W
  // Only pins that can be used as a GPIO are listed.
  enum class PinNum
  {
    W8 = 8, // H3 I2C pin, on-board 1.8 kOhm pull up (do not have to enable internal pull up)
      W9 = 9, // H5 I2C pin, on-board 1.8 kOhm pull up (do not have to enable internal pull up)
      W7 = 7, // H7 GPIO, GPCLK0. Used by one-wire kernel driver.
      W15 = 15, // H8 UART pin (disable in OS)
      W16 = 16, // H10 UART pin (disable in OS)
      W0 = 0, // H11 GPIO
      W1 = 1, // H12 GPIO. Can be set to PWM but then one of the analog audio outputs will be present on this pin
      W2 = 2, // H13 GPIO, SDIO
      W3 = 3, // H15 GPIO, SDIO
      W4 = 4, // H16 GPIO, SDIO
      W5 = 5, // H18 GPIO, SDIO
      W12 = 12, // H19 GPIO, SPI pin
      W13 = 13, // H21 GPIO, SPI pin
      W6 = 6, // H22 GPIO, SDIO
      W14 = 14, // H23 GPIO, SPI pin
      W10 = 10, // H24 GPIO, SPI pin
      W11 = 11, // H26 GPIO, SPI pin
      W30 = 30, // H27 DO NOT USE! Reserved for external EEPROM comms
      W31 = 31, // H28 DO NOT USE! Reserved for external EEPROM comms
      W21 = 21, // H29 GPIO, GPCLK1
      W22 = 22, // H31 GPIO, GPCLK1
      W26 = 26, // H32 GPIO, PWM0
      W23 = 23, // H33 GPIO, PWM1
      W24 = 24, // H35 GPIO, SPI
      W27 = 27, // H36 GPIO, SPI
      W25 = 25, // H37 GPIO, SDIO
      W28 = 28, // H38 GPIO, SPI
      W29 = 29, // H40 GPIO, SPI
      Unknown = 32
      };

  namespace PIN_UTILS
  {
    void setPullMode(int pin, PullMode pull);
    void setPullMode(PinNum pin, PullMode pull);
    bool isNumInPinRange(int num);
  };
};

#endif
