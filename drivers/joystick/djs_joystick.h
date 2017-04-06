#ifndef DJS_JOYSTICK_H
#define DJS_JOYSTICK_H

#include <future>
#include "djs_common.h"

namespace D_JS
{
  class JoyStickOwner
  {
  public:
    JoyStickOwner() = default;
  
  private:
    friend class JoyStick;
    //these functions must be thread safe
    virtual void handleEvent(const JSEvent &event) = 0;
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
    //should be called after init().
    //runs a read thread and calls owner callback to handle event details

    void stop();

  private:
    bool openDevice();
    void closeDevice();
    void stopThread();
    //return 1 implies successful, otherwise an error

    bool readEvent() const;
    //return 1 implies successful, otherwise an error

    static void threadFunc(std::future<bool> shutdown,
			   const JoyStick *const js);
    
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
