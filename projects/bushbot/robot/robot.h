#ifndef ROBOT_H
#define ROBOT_H

#include "kn_basic.h"
#include "coms_serialjoystick.h"
#include "core_servo.h"
#include <memory>

class Robot final : public KERN::KernBasicComponent
{
 public:
  struct Params
  {
  Params(unsigned s_pin,
	 unsigned m_pin)
  : steering_pin(s_pin),
      motor_pin(m_pin)
    {}

    unsigned steering_pin;
    unsigned motor_pin;
    //unsigned roll_pin;
    //unsigned pitch_pin;
    //unsigned yaw_pin;
  };
  
 public:
  explicit Robot(Params&);
  ~Robot();
  
  Robot(const Robot&) = default;
  Robot& operator=(const Robot&) = default;

  // call this function only once before using this class, if wiringPiSetup() from libwiringPi has not been called before.
  static void setup(); 

  bool init(const char* serial_port, int baud=9600);

 private:
  virtual bool stayRunning();

  void processEvent(const JS::JSEventMinimal &event);
  void processButton(const JS::JSEventMinimal &event);
  void processAxis(const JS::JSEventMinimal &event);

 private:
  COMS::JoystickReceiver d_js_receiver;
  JS::JSEventMinimal d_js_event;

  unsigned d_steer_pin = 1;
  unsigned d_motor_pin = 7;
  std::unique_ptr<CORE::Servo> d_steering;
  std::unique_ptr<CORE::Servo> d_motor;
};

#endif
