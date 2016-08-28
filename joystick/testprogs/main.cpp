#include "js_joystick.h"
#include "js_common.h"

//#include <stdint.h>
//#include <stdio.h>
//#include "filter.h"

#include <iostream>
#include <cstdlib> //exit success/failure
#include <memory> //std pointers

class Test : public JS::JoyStickOwner
{
public:
  bool d_error = false;

  Test()
  {
    d_js.reset(new JS::JoyStick(this,"/dev/input/js0"));
  }

  void init()
  {
    std::cout << "init() = " << d_js->init() << std::endl;
    d_js->run();
  }

private:
  virtual void handleReadError() override
  {
    d_error = true;
    std::cout << "read error" << std::endl;
    d_js->stop();
  }

  virtual void handleEvent(const JS::JSEvent &event) override
  {
    std::cout << "time: " << event.time << std::endl;
    std::cout << "value: " << event.value << std::endl;
    std::cout << "type: " << (int)event.type << std::endl;
    std::cout << "number: " << (int)event.number << std::endl;
  }

private:
  std::unique_ptr<JS::JoyStick> d_js = nullptr;
};

int main(int argc, char *argv[])
{
  Test test;
  test.init();

  while (test.d_error == false) {}

  return EXIT_SUCCESS;
}
