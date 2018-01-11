#include "kn_asiokernel.h"
#include "kn_asiocallbacktimer.h"

#include <cassert>
#include <cstdlib>

using namespace KERN;

thread_local static AsioKernel* s_singleton = nullptr;

static const boost::posix_time::time_duration s_cont_time = boost::posix_time::seconds(60);

//----------------------------------------------------------------------//
AsioKernel::AsioKernel()
  : d_timer(d_io, s_cont_time)
{
  assert(s_singleton==nullptr);
  s_singleton = this;

  d_timer.async_wait([&](const boost::system::error_code& e){
      keepBusy(e,d_timer);
    });
}

//----------------------------------------------------------------------//
boost::asio::io_service& AsioKernel::getService()
{
  assert(s_singleton != nullptr);
  return s_singleton->d_io;
}

//----------------------------------------------------------------------//
void AsioKernel::keepBusy(const boost::system::error_code& err,
			  boost::asio::deadline_timer& t)
{
  // check error here
  // do nothing except keep the timer going to keep the service busy
  
  t.expires_at(t.expires_at() + s_cont_time);
  t.async_wait([&](const boost::system::error_code& e){
      keepBusy(e,t);
    });
}
  
//----------------------------------------------------------------------//
void AsioKernel::run()
{
  d_io.run();
  assert(false); // should always be running
}
