#ifndef ROBOT_H
#define ROBOT_H

#include "kn_timer.h"
#include "dgp_controller.h"
#include "djs_serialjoystick.h"

#include <memory>

namespace CTRL
{
  class Servo;
};

class Robot final : KERN::KernelTimerOwner,
  D_GP::GoProControllerOwner
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

  bool init(const std::string& serial_port, int baud=9600);

 private:
  //D_GP::GoProController
  void handleFailedRequest(D_GP::GoProController*) override;

  // KERN::KernelTimerOwner
  bool handleTimeOut(const KERN::KernelTimer& timer);

 private:
  bool processEvent(const D_JS::JSEventMinimal &event);
  bool processButton(const D_JS::JSEventMinimal &event);
  bool processAxis(const D_JS::JSEventMinimal &event);
  void stopMoving();

 private:
  D_JS::JoystickReceiver d_js_receiver;
  D_JS::JSEventMinimal d_js_event;

  unsigned d_steer_num = 2;
  unsigned d_motor_num = 0;
  std::unique_ptr<CTRL::Servo> d_steering;
  std::unique_ptr<CTRL::Servo> d_motor;
  std::unique_ptr<D_GP::GoProController> d_gp_cont;
  
  std::unique_ptr<KERN::KernelTimer> d_process_timer;
  std::unique_ptr<KERN::KernelTimer> d_watchdog_timer;
};

#endif
