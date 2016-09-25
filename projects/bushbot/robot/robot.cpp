#include "robot.h"

#include <iostream>

static const UTIL::Map s_lever_to_servo_pos(JS::EventMinimal::lever_max_in, 
					    JS::EventMinimal::lever_max_out, 
					    CORE::Servo::max_pos, 
					    CORE::Servo::min_pos);

static const UTIL::Map s_stick_to_steer_servo_pos(JS::EventMinimal::axis_max_right, 
					    JS::EventMinimal::axis_max_left, 
					    CORE::Servo::max_pos, 
					    CORE::Servo::min_pos);

//----------------------------------------------------------------------//
Robot::Robot(Params& params)
  : d_steer_pin(params.steering_pin),
    d_motor_pin(params.motor_pin),
    d_steering(CORE::Servo(d_steer_pin)),
    d_motor(CORE::Servo(d_motor_pin))
{}

//----------------------------------------------------------------------//
Robot::~Robot()
{
  d_steering.stop();
  d_motor.stop();
}

//----------------------------------------------------------------------//
void Robot::setup()
{
  CORE::Servo::setup();
}

//----------------------------------------------------------------------//
bool Robot::init(const char* serial_port, 
		 int baud)
{
  bool ret = d_js_receiver.init(serial_port, baud);

  if (ret)
    {
      d_steering.run();
      d_motor.run();
    }

  return ret;
}

//----------------------------------------------------------------------//
bool Robot::stayRunning()
{
  if (d_js_receiver.readSerialEvent(d_js_event))
    {
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
	int servo_sig = static_cast<int>(event.value);
	std::cout << "servo signal = " << servo_sig << std::endl;
	d_steering.setPos(UTIL::mapFromTo(s_stick_to_steer_servo_pos, servo_sig));
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
	int motor_sig = static_cast<int>(event.value);
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor.setPos(UTIL::mapFromTo(s_lever_to_servo_pos, motor_sig));
      }
      break;

    case LT:
      {
	std::cout << "move backward" << std::endl;
	int motor_sig = static_cast<int>(event.value);
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor.setPos(UTIL::mapFromTo(s_lever_to_servo_pos, motor_sig));
      }
      break;

    default:
      break;
    }
}
