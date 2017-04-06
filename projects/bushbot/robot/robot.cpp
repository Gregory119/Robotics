#include "robot.h"
#include "core_hardservo.h"
#include <iostream>

static const unsigned s_steer_servo_range_deg = 180;
static const unsigned s_steer_min_pos_deg = 0;
static const unsigned s_steer_max_pos_deg = 180;
static const unsigned s_steer_min_pos_servo = CORE::Servo::getMinPos()+
  CORE::Servo::getRangePos()*s_steer_min_pos_deg/s_steer_servo_range_deg;
static const unsigned s_steer_max_pos_servo = CORE::Servo::getMinPos()+
  CORE::Servo::getRangePos()*s_steer_max_pos_deg/s_steer_servo_range_deg;
static const unsigned s_steer_mid_pos_servo = 
  s_steer_min_pos_servo + CORE::Servo::getRangePos()/2;

static const UTIL::Map s_rt_lever_to_servo_pos(D_JS::EventMinimal::lever_max_in, 
					       D_JS::EventMinimal::lever_max_out, 
					       CORE::Servo::getMaxPos(), 
					       s_steer_mid_pos_servo);

static const UTIL::Map s_lt_lever_to_servo_pos(D_JS::EventMinimal::lever_max_out, 
					       D_JS::EventMinimal::lever_max_in, 
					       s_steer_mid_pos_servo, 
					       CORE::Servo::getMinPos());

static const UTIL::Map s_stick_to_steer_servo_pos(D_JS::EventMinimal::axis_max_right, 
						  D_JS::EventMinimal::axis_max_left, 
						  s_steer_max_pos_servo, 
						  s_steer_min_pos_servo);

//----------------------------------------------------------------------//
Robot::Robot(Params& params)
  : d_steer_num(params.steering_num),
    d_motor_num(params.motor_num),
    d_steering(new CORE::HardServo(d_steer_num)),
    d_motor(new CORE::HardServo(d_motor_num))
{
  d_steering->moveToPos(s_steer_mid_pos_servo); //start servo in the middle of the set range of motion
}

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
void Robot::processEvent(const D_JS::JSEventMinimal &event)
{
  switch (event.type)
    {
    case D_JS::BUTTON:
      processButton(event);
      break;

    case D_JS::AXIS:
      processAxis(event);
      break;
      
    default:
      std::cout << "Event type not found." << std::endl;
      break;
    }
}

//----------------------------------------------------------------------//
void Robot::processButton(const D_JS::JSEventMinimal &event)
{    
  using namespace D_JS;
    
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
      std::cout << "Button not found." << std::endl;
      break;
    }
}

//----------------------------------------------------------------------//
void Robot::processAxis(const D_JS::JSEventMinimal &event)
{
  using namespace D_JS;
    
  switch (event.number)
    {
    case X1:
      {
	std::cout << "turn left/right" << std::endl;
	unsigned servo_sig = UTIL::mapFromTo(s_stick_to_steer_servo_pos, static_cast<unsigned>(event.value));
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
	unsigned motor_sig = UTIL::mapFromTo(s_rt_lever_to_servo_pos, static_cast<unsigned>(event.value));
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor->moveToPos(motor_sig);
      }
      break;

    case LT:
      {
	std::cout << "move backward" << std::endl;
        unsigned motor_sig = UTIL::mapFromTo(s_lt_lever_to_servo_pos, static_cast<unsigned>(event.value));
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor->moveToPos(motor_sig);
      }
      break;

    default:
      break;
    }
}
