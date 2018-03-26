#ifndef UTIL_SYNCEDTHREADPIPE_H
#define UTIL_SYNCEDTHREADPIPE_H

#include "kn_asiocallbacktimer.h"
#include "utl_threadpipe.h"

namespace UTIL
{
  template <class T>
    class SyncedThreadPipe final
  {
  public:
    SyncedThreadPipe(std::function<void(T)> callback,
		     unsigned max_size = 100);

    void pushBack(T);
    
  private:
    KERN::AsioCallbackTimer d_timer;
    std::unique_ptr<ThreadPipe<T>> d_pipe;
  };

  //----------------------------------------------------------------------//
  template <class T>
    SyncedThreadPipe<T>::SyncedThreadPipe(std::function<void(T)> callback,
					  unsigned max_size)
    : d_pipe(new ThreadPipe<T>(max_size))
  {
    d_timer.setTimeoutCallback([this,callback](){
	while (!d_pipe->empty())
	  {
	    T val;
	    if (d_pipe->tryPopFront(val))
	      {
		callback(std::move(val));
	      }
	  }
      });
    }

  //----------------------------------------------------------------------//
  template <class T>
    void SyncedThreadPipe<T>::pushBack(T val)
    {
      d_pipe->pushBack(std::move(val));
      if (d_pipe->size() <= 1)
	{
	  d_timer.singleShotZero();
	}
    }
};

#endif
