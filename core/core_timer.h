#ifndef CORE_TIMER_H
#define CORE_TIMER_H

#include <memory>

namespace boost
{
  namespace posix_time
  {
    class ptime;
    class time_duration;
  };
};

namespace CORE
{
  class Timer
  {
  public:
    Timer();
    virtual ~Timer();
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    
    void start();
    void stop();
    void reset();
    void resetAndStart();
    long getTimeMs(); //does not check for overflow

  private:
    enum State
    {
      Reset,
      Started,
      Stopped
    };
    
  private:
    State d_state = Reset;
    std::unique_ptr<boost::posix_time::ptime> d_start_ptime;
    std::unique_ptr<boost::posix_time::ptime> d_end_ptime;
    std::unique_ptr<boost::posix_time::time_duration> d_duration;
  };
};

#endif
