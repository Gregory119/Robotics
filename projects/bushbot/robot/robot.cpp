#include "robot.h"

#include <iostream>

//----------------------------------------------------------------------//
Robot::Robot(Params& params)
  : d_js_receiver(COMS::JoystickReceiver(params.wait_error_time_ms)),
    d_steer_pin(params.steering_pin),
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
      std::cout << "take a picture" << std::endl;
      break;
	
    case B:
      //start/stop video recording
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
      std::cout << "turn left/right" << std::endl;
      std::cout << "value = " << (int)event.value << std::endl;
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
      std::cout << "move forward" << std::endl;
      std::cout << "value = " << (int)event.value << std::endl;
      break;

    case LT:
      std::cout << "move backward" << std::endl;
      std::cout << "value = " << (int)event.value << std::endl;
      break;

    default:
      break;
    }
}
