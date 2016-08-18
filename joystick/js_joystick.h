#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <future>

namespace JS_CO
{
class ButtonEvent;
class AxisEvent;
};

class JoyStickOwner
{
 public:
  JoyStickOwner() = default;
  
 private:
  friend class JoyStick;
  virtual void handleButton(JS_CO::ButtonEvent&) = 0;
  virtual void handleAxis(JS_CO::AxisEvent&) = 0;
  virtual void handleReadError() = 0;
};

class JoyStick final
{
 public:
  JoyStick(JoyStickOwner* o, const char *dev_name);
  ~JoyStick(); //closes joystick

  bool init();
  //must open successfully before calling other functions

  void run();
  //runs a read thread and calls owner callback to handle event details

  void stop();

 private:
  bool readEvent() const;
  static void threadFunc(std::future<bool> shutdown,
			 const JoyStick* js);
    
 private:
  static const int s_error = -1;

  JoyStickOwner* d_owner = nullptr;
  const char *d_dev_name = nullptr;
  int d_js_desc = s_error;

  bool d_open = false;
  bool d_running = false;

  std::promise<bool> d_thread_shutdown;
};

#endif
