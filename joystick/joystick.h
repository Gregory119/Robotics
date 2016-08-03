#ifndef JOYSTICK_H
#define JOYSTICK_H

class JoystickOwner
{
 public:
  JoystickOwner() = default;
  
 private:
  friend class JoyStick;
  virtual void handleButton() = 0;
  virtual void handleAxis() = 0;
};

class JoyStick final
{
 public:
  JoyStick(const char *dev_name);
  ~Joystick();

  bool open();
  //must open successfully before calling other functions

  void run();
  //runs a read thread and calls owner callback to handle event details

 private:
  //read()

 private:
  char *d_dev_name;
  int d_js_desc;

  bool d_open = false;
};

#endif
