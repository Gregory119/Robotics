#ifndef DJS_SERIALJOYSTICK_H
#define DJS_SERIALJOYSTICK_H

#include "djs_joystick.h"
#include "djs_common.h"
#include "kn_basic.h"

#include <thread>

namespace UTIL
{
  struct Map;
}

namespace D_JS
{
  class JoystickReceiver;
  
  class JoystickTransmitter : public D_JS::JoyStickOwner,
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
    virtual void handleEvent(const D_JS::JSEvent &event) override; 
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
      char mapToChar(T value, 
		     unsigned max_digits)
      {
	char ret = 0;

	if (max_digits == s_u32_max_digits)
	  {
	    mapFromTo(s_time_to_uchar_map, value, ret);
	  }
	else if (max_digits == s_s16_max_digits)
	  {
	    mapFromTo(s_value_to_char_map, value, ret);
	  }
	else if (max_digits == s_u8_max_digits)
	  {
	    ret = value;
	  }

	return ret;
      }

  private:
    std::mutex m;
    std::unique_ptr<D_JS::JoyStick> d_js = nullptr;
    int d_desc = -1;
    bool d_stay_running = true;
  };

  class JoystickReceiver
  {
  public:
    JoystickReceiver();
    //if there is a serial event read error,  the read will wait for a time of wait_error_ms, followed by cleaning the received serial events in an attempt to avoid further read errors
    JoystickReceiver(const JoystickReceiver&) = default;
    JoystickReceiver& operator=(const JoystickReceiver &copy) = default;
    virtual ~JoystickReceiver();

    bool init(const char* serial_port, 
	      int baud);
    //must return true before calling other functions

    bool readSerialEvent(D_JS::JSEventMinimal &js_event);
    
  private:
    int d_desc = -1;
    D_JS::JSEventMinimal d_js_event; //values auto initialised to zero

    static const UTIL::Map s_uchar_time_to_uint16_map;

    unsigned d_wait_error_ms = 0;
  };
};

#endif
