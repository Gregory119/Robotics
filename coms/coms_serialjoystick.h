#ifndef COMS_SERIALJOYSTICK_H
#define COMS_SERIALJOYSTICK_H

#include "js_joystick.h"
#include "js_common.h"
#include "kn_basic.h"

namespace UTIL
{
  struct Map;
}

namespace COMS
{
  class JoystickTransmitter : public JS::JoyStickOwner,
    public KERN::KernBasicComponent
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

  private:
    static const unsigned s_u32_max_digits;
    static const unsigned s_s16_max_digits;
    static const unsigned s_u8_max_digits;

    static const unsigned s_max_time_ms;
    static const UTIL::Map s_time_to_uchar_map;
    static const UTIL::Map s_value_to_char_map;

  private:
    //----------------------------------------------------------------------//
    template <typename T>
      char convertValueTo8Bits(T value, 
			       unsigned max_digits)
      {
	char ret;

	if (max_digits == s_u32_max_digits)
	  {
	    ret = mapFromTo(s_time_to_uchar_map, value);
	  }
	else if (max_digits == s_s16_max_digits)
	  {
	    ret = mapFromTo(s_value_to_char_map, value);
	  }
	else if (max_digits == s_u8_max_digits)
	  {
	    ret = value;
	  }

	return ret;
      }

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
