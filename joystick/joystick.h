#ifndef JOYSTICK_H
#define JOYSTICK_H

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

  //private:
  bool readEvent();
    
 private:
  static const int s_error = -1;

  JoyStickOwner* d_owner = nullptr;
  const char *d_dev_name = nullptr;
  int d_js_desc = s_error;

  bool d_open = false;
};

#endif
