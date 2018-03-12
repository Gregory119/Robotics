#ifndef CORE_THREADPIPE_H
#define CORE_THREADPIPE_H

#include <mutex>
#include <list>

namespace CORE
{
  // There is a risk of dropping data if the pop rate is slower than push rate
  template <class T>
    class ThreadPipe final
    {
    public:
      explicit ThreadPipe(unsigned max_size = 100);
      ThreadPipe(const ThreadPipe<T>& rhs);
      ThreadPipe<T>& operator=(const ThreadPipe<T>& rhs);
      ThreadPipe(ThreadPipe&& rhs);
      ThreadPipe& operator=(ThreadPipe&& rhs);

      bool empty() const;
      void pushBack(const T& val);

      template <class... Args>
	void emplaceBack(Args&&... args);
      
      bool tryPopFront(T& val);
      unsigned dataDropCount() const;

    private:
      mutable std::mutex d_m; // mutable for use in const functions
      unsigned d_max_size = 100;   
      unsigned d_data_drop_count = 0;
      
      std::list<T> d_data;
    };
};
#endif
