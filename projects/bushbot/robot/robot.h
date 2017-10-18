#ifndef ROBOT_H
#define ROBOT_H

#include "kn_asiocallbacktimer.h"
#include "dgp_fastcontroller.h"
#include "djs_serialjoystick.h"
//#include "utl_mapping.h"

#include <memory>

namespace CTRL
{
  class Servo;
};

class Robot final : D_GP::FastController::Owner
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
  // D_GP::FastController
  void handleFailedRequest(D_GP::GoProController*, D_GP::GoProControllerReq) override;
  void handleSuccessfulRequest(D_GP::GoProController*, D_GP::GoProControllerReq) override;

 private:
  // timer callbacks
  void process();
  
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
  
  KERN::AsioCallbackTimer d_process_timer;
  KERN::AsioCallbackTimer d_watchdog_timer;

  UTIL::Map d_rt_map;
  UTIL::Map d_lt_map;
  UTIL::Map d_stick_map;
};

#endif
