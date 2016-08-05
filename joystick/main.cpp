#include "joystick.h"
#include "common.h"

//#include <stdint.h>
//#include <stdio.h>
//#include "filter.h"

#include <iostream>
#include <cstdlib> //exit success/failure
#include <memory> //std pointers

class Test : public JoyStickOwner
{
public:
  Test()
  {
    d_js.reset(new JoyStick(this,"/dev/input/js0"));
  }

  void init()
  {
    d_js->init();
  }

  void readTest()
  {
    d_js->readEvent();
  }

private:
  virtual void handleButton(JS_CO::ButtonEvent& event) override
  {
    using namespace JS_CO;

    ButtonID id = event.getID();
    switch (id)
      {
      case A:
	std::cout << "A" << event.getValue() << std::endl;
	break;

      case B:
	std::cout << "B" << event.getValue() << std::endl;
	break;

      case X:
	std::cout << "X" << event.getValue() << std::endl;
	break;

      case Y:
	std::cout << "Y" << event.getValue() << std::endl;
	break;

      case LB:
	std::cout << "LB" << event.getValue() << std::endl;
	break;

      case RB:
	std::cout << "RB" << event.getValue() << std::endl;
	break;

      case BACK:
	std::cout << "BACK" << event.getValue() << std::endl;
	break;

      case START:
	std::cout << "START" << event.getValue() << std::endl;
	break;

      case GUIDE:
	std::cout << "GUIDE" << event.getValue() << std::endl;
	break;

      case TL:
	std::cout << "TL" << event.getValue() << std::endl;
	break;

      case TR:
	std::cout << "TR" << event.getValue() << std::endl;
	break;

      default:
	//unknown
	break;
      }
  }

  virtual void handleAxis(JS_CO::AxisEvent& event) override
  {
    using namespace JS_CO;
    switch (event.getID())
      {
      case X1:
	std::cout << "X1 : " << event.getValue() << std::endl;
	break;

      case Y1:
	std::cout << "Y1 : " << event.getValue() << std::endl;
	break;

      case X2:
	std::cout << "X2 : " << event.getValue() << std::endl;
	break;

      case Y2:
	std::cout << "Y2 : " << event.getValue() << std::endl;
	break;

      case RT:
	std::cout << "RT : " << event.getValue() << std::endl;
	break;

      case LT:
	std::cout << "LT : " << event.getValue() << std::endl;
	break;

      default:
	break;
      }
  }

private:
  std::unique_ptr<JoyStick> d_js = nullptr;
};

int main(int argc, char *argv[])
{
  Test test;
  test.init();

  while (1)
    {
      test.readTest();
    }

  return EXIT_SUCCESS;
}
