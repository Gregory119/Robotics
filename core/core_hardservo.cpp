#include "core_hardservo.h"

#include "utl_mapping.h"

#include <iostream>
#include <fstream>

#include <assert.h>

using namespace CORE;
 
static const std::string s_driver_dir = "/dev/servoblaster";
static const unsigned s_max_servo_pin = 7;
static const unsigned s_min_pulse = 50; //each unit is 10 us
static const unsigned s_max_pulse = 250; //each unit is 10 us

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
  unsigned pos_conv = mapFromTo(getPosMap(), pos);
  //write to driver directory
  servo_file.open(s_driver_dir);
  if (servo_file.fail)
    {
      //log
      std::cout << "Failed to open servoblaster driver file. Ensure that servoblaster is running." << std::endl;
      assert(false);
    }
  servo_file << d_servo_num << "=" << pos_conv << std::endl;
  servo_file.close();
}

//----------------------------------------------------------------------//
bool HardServo::incrementMove(uint8_t pos)
{
  if (!isPosInRange(pos+getPos()))
    {
      //print a warning here
      moveToPos(getMaxPos());
      return false;
    }
  else
    {
      moveToPos(getPos()+pos);
    }
  return true;
}

//----------------------------------------------------------------------//
bool HardServo::decrementMove(uint8_t pos)
{
  if (!isPosInRange(getPos()-pos))
    {
      //print a warning here
      moveToPos(getMinPos());
      return false;
    }
  else
    {
      moveToPos(getPos()-pos);
    }
  return true;
}

//----------------------------------------------------------------------//
//HardServo::
