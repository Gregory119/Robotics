#include "ctrl_hardservo.h"

#include "utl_mapping.h"

#include <iostream>
#include <fstream>

#include <cassert>

using namespace CTRL;
 
static const std::string s_driver_dir = "/dev/servoblaster";
static const unsigned s_max_servo_pin = 7;
static const unsigned s_blaster_to_us = 10; //each blaster unit is 10 us

//----------------------------------------------------------------------//   
HardServo::HardServo(unsigned servo_num,
		     int max_pos,
		     int min_pos)
  : Servo(max_pos,min_pos)
{
  assert(servo_num <= s_max_servo_pin);
  d_servo_num = servo_num;
}

//----------------------------------------------------------------------//
void HardServo::moveToPos(int pos)
{
  assert(isPosValid(pos));
  
  if (!d_first_move)
    {
      setReqPos(pos);
    }
  else
    {
      moveToStartPos(pos);
      d_first_move = false;
    }

  updateMove();
}

//----------------------------------------------------------------------//
void HardServo::updateMove()
{
  unsigned pos_blast = getSetPulseUs()/s_blaster_to_us;

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
