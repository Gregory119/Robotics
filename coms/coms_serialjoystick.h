//coms for joystick receiver
//coms for joystick transmitter
#ifndef COMS_SERIALJOYSTICK_H
#define COMS_SERIALJOYSTICK_H

#include "js_joystick.h"
#include "js_common.h"
#include "kn_basic.h"

namespace COMS
{
  class JoystickTransmitter : public JS::JoyStickOwner,
    public KN::KernBasicComponent
  {
  public:
    JoystickTransmitter(const char* serial_port, 
			int baud,
			const char* js_source);
    virtual ~JoystickTransmitter();

  private:
    virtual void handleEvent(const js_event &event) override; 
    virtual void handleReadError() override;
    virtual bool stayRunning() override;

    std::string convertValueToString(double value,
				     unsigned max_digits);

  private:
    std::unique_ptr<JS::JoyStick> d_js = nullptr;
    int d_desc = -1;
    std::string d_serial_cmd;
    bool d_stay_running = true;
  };

  class JoystickReceiver;

  class JoystickReceiverOwner
  {
  private:
    friend JoystickReceiver;
    virtual void handleButtonEvent(const JS::ButtonEvent &) = 0;
    virtual void handleAxisEvent(const JS::AxisEvent &) = 0;
  };

  class JoystickReceiver
  {
  public:
    void readEvent();
    
  private:
    bool readSerialPort();
  };
};

#endif
