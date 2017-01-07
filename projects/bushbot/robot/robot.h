#ifndef ROBOT_H
#define ROBOT_H

#include "kn_basic.h"
#include "coms_serialjoystick.h"
#include <memory>

class HardServo;

class Robot final : public KERN::KernBasicComponent
{
 public:
  struct Params
  {
  Params(unsigned s_num,
	 unsigned m_num)
  : steering_num(s_num),
      motor_num(m_num)
    {}

    unsigned steering_num;
    unsigned motor_num;
    //unsigned roll_num;
    //unsigned pitch_num;
    //unsigned yaw_num;
  };
  
 public:
  explicit Robot(Params&);
  ~Robot();
  
  Robot(const Robot&) = default;
  Robot& operator=(const Robot&) = default;

  bool init(const char* serial_port, int baud=9600);

 private:
  virtual bool stayRunning();

  void processEvent(const JS::JSEventMinimal &event);
  void processButton(const JS::JSEventMinimal &event);
  void processAxis(const JS::JSEventMinimal &event);

 private:
  COMS::JoystickReceiver d_js_receiver;
  JS::JSEventMinimal d_js_event;

  unsigned d_steer_num = 2;
  unsigned d_motor_num = 0;
  std::unique_ptr<Servo> d_steering;
  std::unique_ptr<Servo> d_motor;
};

#endif
