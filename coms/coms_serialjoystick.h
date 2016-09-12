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
  class JoystickReceiver;

  class JoystickTransmitter : public JS::JoyStickOwner,
    public KERN::KernBasicComponent
  {
    friend JoystickReceiver; //allow access to the static private constants

  public:
    JoystickTransmitter();
    JoystickTransmitter(const JoystickTransmitter&) = delete; //cannot use copy constructor
    JoystickTransmitter& operator=(const JoystickTransmitter&) = delete; //cannot copy
    virtual ~JoystickTransmitter();

    bool init(const char* serial_port, 
	      int baud,
	      const char* js_source);
    //must return true before calling other functions

  private:
    virtual void handleEvent(const JS::JSEvent &event) override; 
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
    std::string d_serial_cmd = "";
    bool d_stay_running = true;
  };

  class JoystickReceiver
  {
  public:
    JoystickReceiver(unsigned wait_error_ms);
    //if there is a serial event read error,  the read will wait for a time of wait_error_ms, followed by cleaning the received serial events in an attempt to avoid further read errors
    JoystickReceiver(const JoystickReceiver&) = default;
    JoystickReceiver& operator=(const JoystickReceiver &copy) = default;
    virtual ~JoystickReceiver();

    bool init(const char* serial_port, 
	      int baud);
    //must return true before calling other functions

    bool readSerialEvent(JS::JSEventMinimal &js_event);
    
  private:
    int d_desc = -1;
    JS::JSEventMinimal d_js_event; //values auto initialised to zero

    static const UTIL::Map s_uchar_time_to_uint16_map;

    unsigned d_wait_error_ms = 0;
  };
};

#endif
