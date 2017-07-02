#include "core_timer.h"

#include "boost/date_time/posix_time/posix_time.hpp"


using namespace CORE;
using namespace boost;

//----------------------------------------------------------------------//
Timer::Timer()
  : d_start_ptime(new posix_time::ptime(posix_time::microsec_clock::local_time())),
    d_end_ptime(new posix_time::ptime(posix_time::microsec_clock::local_time())),
  d_duration(new posix_time::time_duration(0,0,0,0)) //hrs, min, secs, nanosecs
{}

//----------------------------------------------------------------------//~
Timer::~Timer() = default;

//----------------------------------------------------------------------//
void Timer::start()
{
  assert(d_state != Started);
  
  switch (d_state)
    {
    case Reset:
*d_start_ptime = posix_time::microsec_clock::local_time();
      break;

    case Stopped:
      //continue to time
      break;

 default:
    assert(false);
    return;
    };
  
  d_state = Started;
}

//----------------------------------------------------------------------//
void Timer::stop()
{
  assert(d_state != Stopped);
  
  d_state = Stopped;
  *d_end_ptime = posix_time::microsec_clock::local_time();
}

//----------------------------------------------------------------------//
void Timer::reset()
{
  d_state = Reset;
  *d_start_ptime = posix_time::microsec_clock::local_time();
  *d_end_ptime = posix_time::microsec_clock::local_time();
}

//----------------------------------------------------------------------//
long Timer::getTimeMs()
{
  switch (d_state)
    {
    case Started:
      *d_end_ptime = posix_time::microsec_clock::local_time();
*d_duration = *d_end_ptime - *d_start_ptime;
      break;
      
    case Stopped:
      //leave end time unchanged
      break;

    case Reset:
 default:
    assert(false);
    return 0;
    };

  return d_duration->total_milliseconds();
}

//----------------------------------------------------------------------//
void Timer::resetAndStart()
{
  reset();
  start();
}
