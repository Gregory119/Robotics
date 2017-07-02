#include "djs_joystick.h"
#include "djs_common.h"

//#include <stdint.h>
//#include <stdio.h>
//#include "filter.h"

#include <iostream>
#include <cstdlib> //exit success/failure
#include <memory> //std pointers

class Test : public D_JS::JoyStickOwner
{
private:
  std::mutex m;

public:
  bool d_error = false;

  Test()
    : d_js(new D_JS::JoyStick(this,"/dev/input/js0"))
  {}

  void init()
  {
    std::cout << "init() = " << d_js->init() << std::endl;
    d_js->run();
  }

private:
  virtual void handleReadError() override
  {
    std::lock_guard<std::mutex> lock(m);
    d_error = true;
    std::cout << "read error" << std::endl;
    d_js->stop();
  }

  virtual void handleEvent(const D_JS::JSEvent &event) override
  {
    std::lock_guard<std::mutex> lock(m);
    std::cout << "time: " << event.time << std::endl;
    std::cout << "value: " << event.value << std::endl;
    std::cout << "type: " << (int)event.type << std::endl;
    std::cout << "number: " << (int)event.number << std::endl;
  }

private:
  std::unique_ptr<D_JS::JoyStick> d_js = nullptr;
};

int main(int argc, char *argv[])
{
  Test test;
  test.init();

  while (test.d_error == false) {}

  return EXIT_SUCCESS;
}
