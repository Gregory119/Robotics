#include "robot.h"
#include "ctrl_hardservo.h"

#include <cassert>
#include <iostream>

static const unsigned s_steer_min_pos_deg = 60;
static const unsigned s_steer_max_pos_deg = 116;
static const unsigned s_steer_mid_pos_deg = 95;
static const unsigned s_steer_abs_max_pos_deg = 180;
static const unsigned s_steer_abs_min_pos_deg = 0;

//----------------------------------------------------------------------//
Robot::Robot(Params& params)
  : d_steer_num(params.steering_num),
    d_motor_num(params.motor_num),
    d_motor(new CTRL::HardServo(d_motor_num)),
    d_gp_cont(new D_GP::FastController
	      (this, D_GP::FastController::CtrlParams()
	       .setName("Robot"))),
    d_rt_map(UTIL::Map(D_JS::axis_max, 
		       D_JS::axis_min, 
		       d_motor->getMaxInputPos(), 
		       d_motor->getMidInputPos())),
    d_lt_map(UTIL::Map(D_JS::axis_max, 
		       D_JS::axis_min, 
		       d_motor->getMidInputPos(), 
		       d_motor->getMinInputPos()))
{  
  //motor
  d_motor->setAsESC();
  d_motor->moveToMidPos();
  d_motor->setVelocityParams(4000, 100);

  //steering
  d_steering.reset(new CTRL::HardServo(d_steer_num));
  d_stick_map = UTIL::Map(D_JS::axis_max,
			  D_JS::axis_min,
			  d_steering->getMaxInputPos(),
			  d_steering->getMinInputPos());

  UTIL::Map pos_deg_to_steer_input(s_steer_abs_max_pos_deg,
				   s_steer_abs_min_pos_deg,
				   d_steering->getMaxInputPos(),
				   d_steering->getMinInputPos());
  d_steering->setOutputPosLimits(pos_deg_to_steer_input.map<int>(s_steer_min_pos_deg), //min
				 pos_deg_to_steer_input.map<int>(s_steer_max_pos_deg), //max
				 pos_deg_to_steer_input.map<int>(s_steer_mid_pos_deg)); //mid
  d_steering->moveToMidPos();

  //timers
  d_process_timer.setTimeoutCallback([this](){
      process();
    });
  d_watchdog_timer.setTimeoutCallback([this](){
      stopMoving();
    });
  
  d_process_timer.restartMs(1);
  d_watchdog_timer.restartMs(1000);
  std::cout << "constructed" << std::endl;
}

//----------------------------------------------------------------------//
Robot::~Robot()
{}

//----------------------------------------------------------------------//
bool Robot::init(const std::string& serial_port, 
		 int baud)
{
  return d_js_receiver.init(serial_port, baud);
}

//----------------------------------------------------------------------//
void Robot::process()
{
  if (d_js_receiver.readSerialEvent(d_js_event))
    {
      if (processEvent(d_js_event))
	{
	  d_watchdog_timer.restart(); 
	}
    }
}

//-----------------------------------------------------------------------//
void Robot::stopMoving()
{
  d_motor->moveToMidPos(); // stop moving/coast (helpful with regenerative braking)
}

//----------------------------------------------------------------------//
bool Robot::processEvent(const D_JS::JSEventMinimal &event)
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
      return false;
    }
  return true;
}

//----------------------------------------------------------------------//
bool Robot::processButton(const D_JS::JSEventMinimal &event)
{
  std::cout << "processButton." << std::endl;
  using namespace D_JS;
    
  switch (event.number)
    {
    case Y:
      std::cout << "Y" << std::endl;
      // should automatically connect
      /*
      if (event.value==1)
	{
	  std::cout << "Trying to connect." << std::endl;
	  d_gp_cont->connect();
	}
      */
      break;

    case A:
      std::cout << "A" << std::endl;
      if (event.value==1)
	{
	  std::cout << "Taking pic." << std::endl;
	  d_gp_cont->takePhotoReq();
	}
      break;
	
    case B:
      std::cout << "B" << std::endl;
      if (event.value==1)
	{
	  std::cout << "Start or Stop Recording" << std::endl;
	  d_gp_cont->toggleRecordingReq();
	}
      break;

    case X:
      std::cout << "X" << std::endl;
      break;
      
    default:
      std::cout << "Button not found." << std::endl;
      return false;
    }
  return true;
}

//----------------------------------------------------------------------//
bool Robot::processAxis(const D_JS::JSEventMinimal &event)
{
  using namespace D_JS;
    
  switch (event.number)
    {
    case X1:
      {
	unsigned servo_sig = 0;
        d_stick_map.map(static_cast<unsigned>(event.value), servo_sig);
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
	unsigned motor_sig = 0;
        d_rt_map.map(static_cast<unsigned>(event.value), motor_sig);
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor->moveToPos(motor_sig);
      }
      break;

    case LT:
      {
        unsigned motor_sig = 0;
        d_lt_map.map(static_cast<unsigned>(event.value), motor_sig);
	motor_sig = d_lt_map.flipOnOutAxis(motor_sig);
	std::cout << "motor signal = " << motor_sig << std::endl;
	d_motor->moveToPos(motor_sig);
      }
      break;

    default:
      return false;
      break;
    }
  return true;
}

//----------------------------------------------------------------------//
void Robot::handleFailedRequest(D_GP::FastController*,
				D_GP::FastController::Req req)
{
  // could re-request the failed command here, but would need a timeout to stop retrying
  std::cout << "Robot::handleFailedRequest: " << std::endl;
}

//----------------------------------------------------------------------//
void Robot::handleSuccessfulRequest(D_GP::FastController*,
				    D_GP::FastController::Req req)
{
  std::cout << "Robot::handleSuccessfulRequest: " << std::endl;
}
