#include "robot.h"
#include "core_hardservo.h"
#include <iostream>

static const UTIL::Map s_lever_to_servo_pos(JS::EventMinimal::lever_max_in, 
					    JS::EventMinimal::lever_max_out, 
					    CORE::Servo::getMaxPos(), 
					    CORE::Servo::getMinPos());

static const UTIL::Map s_stick_to_steer_servo_pos(JS::EventMinimal::axis_max_right, 
						  JS::EventMinimal::axis_max_left, 
						  CORE::Servo::getMaxPos(), 
						  CORE::Servo::getMinPos());

//----------------------------------------------------------------------//
Robot::Robot(Params& params)
  : d_steer_num(params.steering_num),
    d_motor_num(params.motor_num),
    d_steering(new CORE::HardServo(d_steer_num)),
    d_motor(new CORE::HardServo(d_motor_num))
{}

//----------------------------------------------------------------------//
Robot::~Robot()
{}

//----------------------------------------------------------------------//
bool Robot::init(const char* serial_port, 
		 int baud)
{
  return d_js_receiver.init(serial_port, baud);
}

//----------------------------------------------------------------------//
bool Robot::stayRunning()
{
  if (d_js_receiver.readSerialEvent(d_js_event))
    {
      std::cout << "received event" << std::endl;
      processEvent(d_js_event);
    }
  else
    {
      //consider actions when no event is received after a prolonged period of time
      //eg. set the motor speed to zero
    }
    
  //no reason to stop running
  return true;
}

//----------------------------------------------------------------------//
void Robot::processEvent(const JS::JSEventMinimal &event)
{
  switch (event.type)
    {
    case JS::BUTTON:
      processButton(event);
      break;

    case JS::AXIS:
      processAxis(event);
      break;
    }
}

//----------------------------------------------------------------------//
void Robot::processButton(const JS::JSEventMinimal &event)
{    
  using namespace JS;
    
  switch (event.number)
    {
    case A:
      //take a picture
      if (event.value==1)
	std::cout << "take a picture" << std::endl;
      break;
	
    case B:
      //start/stop video recording
      if (event.value==1)
	std::cout << "start/stop video recording" << std::endl;
      break;

    default:
      break;
    }
}

//----------------------------------------------------------------------//
void Robot::processAxis(const JS::JSEventMinimal &event)
{
  using namespace JS;
    
  switch (event.number)
    {
    case X1:
      {
	std::cout << "turn left/right" << std::endl;
	int servo_sig = UTIL::mapFromTo(s_stick_to_steer_servo_pos, static_cast<int>(event.value));
	std::cout << "servo signal = " << servo_sig << std::endl;
	d_steering->moveToPos(servo_sig);
      }
      break;
	
    case X2:
      std::cout << "look left/right" << std::endl;
      std::cout << "value = " << (int)event.value << std::endl;
      break;

    case Y2:
      std::cout << "look up/down" << std::endl;
      std::cout << "value = " << (int)event.value << std::endl;
      break;

    case RT:
      {
	std::cout << "move forward" << std::endl;
	int motor_sig = UTIL::mapFromTo(s_lever_to_servo_pos, static_cast<int>(event.value));
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor->moveToPos(motor_sig);
      }
      break;

    case LT:
      {
	std::cout << "move backward" << std::endl;
	int motor_sig = -UTIL::mapFromTo(s_lever_to_servo_pos, static_cast<int>(event.value));
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor->moveToPos(motor_sig);
      }
      break;

    default:
      break;
    }
}
