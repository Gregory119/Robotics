#ifndef DJS_SERIALJOYSTICK_H
#define DJS_SERIALJOYSTICK_H

#include "djs_common.h"
#include "djs_joystick.h"

#include "kn_timer.h"
#include "utl_mapping.h"

#include <thread>

namespace D_JS
{
  class JoystickReceiver;  
  class JoystickTransmitter : JoyStickOwner,
    KERN::KernelTimerOwner
  {
  public:
    JoystickTransmitter();
    JoystickTransmitter(const JoystickTransmitter&) = delete; //cannot use copy constructor
    JoystickTransmitter& operator=(const JoystickTransmitter&) = delete; //cannot copy
    virtual ~JoystickTransmitter();

    bool init(const std::string& serial_port, 
	      int baud,
	      const std::string& js_source);
    //must return true before calling other functions

    // has no affect if already started
    void setResendEventTimeoutMs(long);
    void enableOnlyResendAxisEvents(bool); // axis only events enabled by default
    void start();
    
  private:
    // KERN::KernelTimerOwner
    bool handleTimeOut(const KERN::KernelTimer& timer);
    
  private:
    virtual void handleEvent(const JSEvent &event) override; 
    virtual void handleReadError() override;
    void sendEvent(const JSEvent &event);
    void resendLastEvent();

  private:
    //----------------------------------------------------------------------//
    template <typename T>
      char mapToChar(T value, 
		     unsigned max_digits)
      {
	char ret = 0;

	if (max_digits == s_u32_max_digits)
	  {
	    s_time_to_uchar_map.map(value, ret);
	  }
	else if (max_digits == s_s16_max_digits)
	  {
	    s_value_to_char_map.map(value, ret);
	  }
	else if (max_digits == s_u8_max_digits)
	  {
	    ret = value;
	  }

	return ret;
      }

  private:
    friend JoystickReceiver; //allow access to the static private constants
    
    std::mutex d_m;
    JSEvent d_resend_event;
    std::unique_ptr<D_JS::JoyStick> d_js;
    int d_desc = -1;
    bool d_is_init = false;

    static const unsigned s_u32_max_digits;
    static const unsigned s_s16_max_digits;
    static const unsigned s_u8_max_digits;

    static const unsigned s_max_time_ms;
    static const UTIL::Map s_time_to_uchar_map;
    static const UTIL::Map s_value_to_char_map;

    bool d_enable_resend_event = false;
    KERN::KernelTimer d_resend_timer;
    long d_resend_time_ms = 60000;
    bool d_resend_only_axis = true;
  };

  class JoystickReceiver
  {
  public:
    JoystickReceiver();
    //if there is a serial event read error,  the read will wait for a time of wait_error_ms, followed by cleaning the received serial events in an attempt to avoid further read errors
    JoystickReceiver(const JoystickReceiver&) = default;
    JoystickReceiver& operator=(const JoystickReceiver &copy) = default;
    virtual ~JoystickReceiver();

    bool init(const std::string& serial_port, 
	      int baud);
    //must return true before calling other functions

    bool readSerialEvent(JSEventMinimal &js_event);
    
  private:
    int d_desc = -1;
    JSEventMinimal d_js_event; //values auto initialised to zero

    static const UTIL::Map s_uchar_time_to_uint16_map;

    unsigned d_wait_error_ms = 0;
  };
};

#endif
