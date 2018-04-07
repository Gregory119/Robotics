#ifndef UTIL_SYNCEDTHREADPIPE_H
#define UTIL_SYNCEDTHREADPIPE_H

#include "kn_safecallbacktimer.h"
#include "utl_threadpipe.h"

namespace UTIL
{
  template <class T>
    class SyncedThreadPipe final
  {
  public:
    SyncedThreadPipe(std::function<void(T&&)> callback,
		     unsigned max_size = 100);

    void pushBack(T&&);
    void pushBack(const T&);
    
  private:
    KERN::SafeCallbackTimer d_timer = KERN::SafeCallbackTimer("UTIL::SyncedThreadPipe Timer");
    std::unique_ptr<ThreadPipe<T>> d_pipe;
    std::function<void(T&&)> d_callback;
  };

  //----------------------------------------------------------------------//
  template <class T>
    SyncedThreadPipe<T>::SyncedThreadPipe(std::function<void(T&&)> callback,
					  unsigned max_size)
    : d_pipe(new ThreadPipe<T>(max_size)),
    d_callback(callback)
  {
    d_timer.setTimeoutCallback([&](){
	while (!d_pipe->empty())
	  {
	    T val;
	    if (d_pipe->tryPopFront(val))
	      {
		d_callback(std::move(val));
	      }
	  }
      });
    }

  //----------------------------------------------------------------------//
  template <class T>
    void SyncedThreadPipe<T>::pushBack(T&& val)
    {
      d_pipe->pushBack(std::move(val));
      if (d_pipe->size() <= 1)
	{
	  d_timer.singleShotZero();
	}
    }

  //----------------------------------------------------------------------//
  template <class T>
    void SyncedThreadPipe<T>::pushBack(const T& val)
    {
      d_pipe->pushBack(val);
      if (d_pipe->size() <= 1)
	{
	  d_timer.singleShotZero();
	}
    }
};

#endif
