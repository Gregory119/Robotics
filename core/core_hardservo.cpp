#include "core_hardservo.h"

#include "utl_mapping.h"

#include <iostream>
#include <fstream>

#include <cassert>

using namespace CORE;
 
static const std::string s_driver_dir = "/dev/servoblaster";
static const unsigned s_max_servo_pin = 7;
static const unsigned s_blaster_to_us = 10; //each blaster unit is 10 us

//----------------------------------------------------------------------//   
HardServo::HardServo(unsigned servo_num)
{
  assert(servo_num <= s_max_servo_pin);
  d_servo_num = servo_num;
}

//----------------------------------------------------------------------//
void HardServo::moveToPos(uint8_t pos)
{
  setPosValue(pos);
  unsigned pos_blast = UTIL::mapFromTo(getPosMap(), pos)/s_blaster_to_us;

  std::ofstream servo_file;
  servo_file.open(s_driver_dir);
  if (servo_file.fail())
    {
      std::cout << "Failed to open servoblaster driver file. Ensure that servoblaster is running." << std::endl;
      assert(false);
    }
  servo_file << d_servo_num << "=" << pos_blast << std::endl;
  servo_file.close();
}

//----------------------------------------------------------------------//
//HardServo::
