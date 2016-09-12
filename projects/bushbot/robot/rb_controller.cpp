#include "kn_basic.h"
#include "coms_serialjoystick.h"

#include <iostream>

class Robot final : public KERN::KernBasicComponent
{
 public:
  Robot(unsigned wait_error_time_ms)
  : d_js_receiver(COMS::JoystickReceiver(wait_error_time_ms))
  {}

  Robot(const Robot&) = default;
  Robot& operator=(const Robot&) = default;

  bool init(const char* serial_port, 
	    int baud=9600)
  {
    return d_js_receiver.init(serial_port,
			      baud);
  }

 private:
  virtual bool stayRunning()
  {
    if (d_js_receiver.readSerialEvent(d_js_event))
      {
	processEvent(d_js_event);
      }
    
    //no reason to stop running
    return true;
  }

  void processEvent(const JS::JSEventMinimal &event)
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

  void processButton(const JS::JSEventMinimal &event)
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

  void processAxis(const JS::JSEventMinimal &event)
  {
    using namespace JS;
    
    switch (event.number)
      {
      case X1:
	std::cout << "turn left/right" << std::endl;
	break;
	
      case X2:
	std::cout << "look left/right" << std::endl;
	break;

      case Y2:
	std::cout << "look up/down" << std::endl;
	break;

      case RT:
	std::cout << "move forward" << std::endl;
	break;

      case LT:
	std::cout << "move backward" << std::endl;
	break;

      default:
	break;
      }
  }

 private:
  COMS::JoystickReceiver d_js_receiver;
  JS::JSEventMinimal d_js_event;
};

int main()
{
  unsigned wait_error_time_ms = 500;
  Robot r(wait_error_time_ms);
  std::string serial_port = "/dev/ttyAMA0";
  int baud=9600;
  if (!r.init(serial_port.c_str(),
	      baud))
    {
      std::cout << "failed to initialize robot" << std::endl;
      return 0;
    }

  KERN::KernBasic k(&r);
  k.run();
  
  return 0;
}
