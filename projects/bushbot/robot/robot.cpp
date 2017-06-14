#include "robot.h"
#include "core_hardservo.h"

#include <cassert>
#include <iostream>

static const unsigned s_steer_min_pos_deg = 50;
static const unsigned s_steer_max_pos_deg = 120;
static const unsigned s_steer_range_deg = 180;
static const unsigned s_steer_min_pos_servo = CORE::Servo::getMinPos()+
  CORE::Servo::getRangePos()*s_steer_min_pos_deg/s_steer_range_deg;
static const unsigned s_steer_max_pos_servo = CORE::Servo::getMinPos()+
  CORE::Servo::getRangePos()*s_steer_max_pos_deg/s_steer_range_deg;
static const unsigned s_steer_mid_pos_servo = 
  (s_steer_min_pos_servo + s_steer_max_pos_servo)/2;

static const unsigned s_esc_mid_pos_servo = 
  (CORE::Servo::getMinPos() + CORE::Servo::getMaxPos())/2;

static const UTIL::Map s_rt_map(D_JS::axis_max, 
				D_JS::axis_min, 
				CORE::Servo::getMaxPos(), 
			        s_esc_mid_pos_servo);

static const UTIL::Map s_lt_map(D_JS::axis_min, 
				D_JS::axis_max, 
				s_esc_mid_pos_servo, 
			        CORE::Servo::getMinPos());

static const UTIL::Map s_stick_map(D_JS::axis_max, 
				   D_JS::axis_min, 
				   s_steer_max_pos_servo, 
				   s_steer_min_pos_servo);

//----------------------------------------------------------------------//
Robot::Robot(Params& params)
  : d_steer_num(params.steering_num),
    d_motor_num(params.motor_num),
    d_steering(new CORE::HardServo(d_steer_num)),
    d_motor(new CORE::HardServo(d_motor_num)),
    d_gp_cont(new D_GP::GoProController(this, D_GP::ControlType::Simple))
{
  d_motor->setUsTiming(1000, 2000); //in microseconds
  d_steering->setUsTiming(500, 2500); //in microseconds
  d_steering->moveToPos(s_steer_mid_pos_servo); //start servo in the middle of the set range of motion
  d_motor->moveToPos(127); //start servo in the middle of the set range of motion
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
      assert(false);
      return;
    }
}

//----------------------------------------------------------------------//
void Robot::processButton(const D_JS::JSEventMinimal &event)
{
  std::cout << "processButton." << std::endl;
  using namespace D_JS;
    
  switch (event.number)
    {
    case Y:
      std::cout << "Y" << std::endl;
      if (event.value==1)
	{
	  std::cout << "Trying to connect." << std::endl;
	  d_gp_cont->connectWithName("Mantis");
	}
      break;

    case A:
      std::cout << "A" << std::endl;
      if (event.value==1)
	{
	  std::cout << "Taking pic." << std::endl;
	  d_gp_cont->takePicture();
	}
      break;
	
    case B:
      std::cout << "B" << std::endl;
      if (event.value==1)
	{
	  std::cout << "Start or Stop Recording" << std::endl;
	  d_gp_cont->StartStopRecording();
	}
      break;

    case X:
      std::cout << "X" << std::endl;
      if (event.value==1)
	{
	  std::cout << "taking multishot" << std::endl;
	  d_gp_cont->takeMultiShot();
	}
      break;
      
    default:
      std::cout << "Button not found." << std::endl;
      return;
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
	unsigned servo_sig = 0;
	UTIL::mapFromTo(s_stick_map, static_cast<unsigned>(event.value), servo_sig);
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
	unsigned motor_sig = 0;
	UTIL::mapFromTo(s_rt_map, static_cast<unsigned>(event.value), motor_sig);
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor->moveToPos(motor_sig);
      }
      break;

    case LT:
      {
	std::cout << "move backward" << std::endl;
        unsigned motor_sig = 0;
	UTIL::mapFromTo(s_lt_map, static_cast<unsigned>(event.value), motor_sig);
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor->moveToPos(motor_sig);
      }
      break;

    default:
      break;
    }
}

//----------------------------------------------------------------------//
void Robot::handleFailedRequest(D_GP::GoProController*, D_GP::Request req)
{
  std::cout << "Robot::handleFailedRequest: " << D_GP::reqToString(req) << std::endl;
}
