#ifndef PWP_PINS_H
#define PWP_PINS_H

namespace P_WP
{
  enum class PullMode
  {
    Up,
      Down,
      None
      };
  
  // Only raspberry pis using a 20x2 pin header are supported:
  // Pi 1A+, 1B+, 2, 3, Zero, Zero W
  enum class PinNum
  {
    H3 = 8, // I2C pin, on-board 1.8 kOhm pull up (do not have to enable internal pull up)
      H5 = 9, // I2C pin, on-board 1.8 kOhm pull up (do not have to enable internal pull up)
      H7 = 7, // GPIO, GPCLK0. Used by one-wire kernel driver.
      H8 = 15, // UART pin (disable in OS)
      H10 = 16, // UART pin (disable in OS)
      H11 = 0, // GPIO
      H12 = 1, // GPIO. Can be set to PWM but then one of the analog audio outputs will be present on this pin
      H13 = 2, // GPIO, SDIO
      H15 = 3, // GPIO, SDIO
      H16 = 4, // GPIO, SDIO
      H18 = 5, // GPIO, SDIO
      H19 = 12, // GPIO, SPI pin
      H21 = 13, // GPIO, SPI pin
      H22 = 6, // GPIO, SDIO
      H23 = 14, // GPIO, SPI pin
      H24 = 10, // GPIO, SPI pin
      H26 = 11, // GPIO, SPI pin
      H27 = 30, // DO NOT USE! Reserved for external EEPROM comms
      H28 = 31, // DO NOT USE! Reserved for external EEPROM comms
      H29 = 21, // GPIO, GPCLK1
      H31 = 22, // GPIO, GPCLK1
      H32 = 26, // GPIO, PWM0
      H33 = 23, // GPIO, PWM1
      H35 = 24, // GPIO, SPI
      H36 = 27, // GPIO, SPI
      H37 = 25, // GPIO, SDIO
      H38 = 28, // GPIO, SPI
      H40 = 29, // GPIO, SPI
      Unknown
      };
};

#endif
