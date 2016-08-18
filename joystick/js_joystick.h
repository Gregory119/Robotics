#ifndef JS_JOYSTICK_H
#define JS_JOYSTICK_H

#include <future>
#include <linux/joystick.h>

namespace JS
{
  class JoyStickOwner
  {
  public:
    JoyStickOwner() = default;
  
  private:
    friend class JoyStick;
    virtual void handleEvent(const js_event &event) = 0;
    virtual void handleReadError() = 0;
  };

  class JoyStick final
  {
  public:
    JoyStick(JoyStickOwner* o, const char *dev_name);
    ~JoyStick(); //closes joystick

    bool init();
    //must open successfully before calling other functions

    void run();
    //runs a read thread and calls owner callback to handle event details

    void stop();

  private:
    bool readEvent() const;
    static void threadFunc(std::future<bool> shutdown,
			   const JoyStick* js);
    
  private:
    static const int s_error = -1;

    JoyStickOwner* d_owner = nullptr;
    const char *d_dev_name = nullptr;
    int d_js_desc = s_error;

    bool d_open = false;
    bool d_running = false;

    std::promise<bool> d_thread_shutdown;
  };
};

#endif
