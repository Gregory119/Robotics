#include "core_hardservo.h"

#include "utl_mapping.h"

#include <iostream>
#include <fstream>

#include <assert.h>

using namespace CORE;
 
static const std::string s_driver_dir = "/dev/servoblaster";
static const unsigned s_max_servo_pin = 7;
static const unsigned s_min_pulse = 100; //each unit is 10 us
static const unsigned s_max_pulse = 200; //each unit is 10 us

static std::ofstream servo_file;

//----------------------------------------------------------------------//   
HardServo::HardServo(unsigned servo_num)
{
  assert(servo_num <= s_max_servo_pin);
  d_servo_num = servo_num;

  setTiming(s_min_pulse, s_max_pulse);
}

//----------------------------------------------------------------------//
void HardServo::moveToPos(uint8_t pos)
{
  setPosValue(pos);
  unsigned pos_conv = UTIL::mapFromTo(getPosMap(), pos);

  servo_file.open(s_driver_dir);
  if (servo_file.fail())
    {
      std::cout << "Failed to open servoblaster driver file. Ensure that servoblaster is running." << std::endl;
      assert(false);
    }
  servo_file << d_servo_num << "=" << pos_conv << std::endl;
  servo_file.close();
}

//----------------------------------------------------------------------//
//HardServo::
