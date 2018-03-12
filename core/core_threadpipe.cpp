#include "core_threadpipe.h"

using namespace CORE;

//----------------------------------------------------------------------//
template <class T>
ThreadPipe<T>::ThreadPipe(unsigned max_size)
  : d_max_size(max_size)
{}

//----------------------------------------------------------------------//
template <class T>
ThreadPipe<T>::ThreadPipe(const ThreadPipe<T>& rhs)
{
  *this = rhs;
}

//----------------------------------------------------------------------//
template <class T>
ThreadPipe<T>& ThreadPipe<T>::operator=(const ThreadPipe<T>& rhs)
{
  if (this == &rhs)
    {
      return *this;
    }
    
  std::lock_guard<std::mutex> lk_rhs(rhs.d_m);
  std::lock_guard<std::mutex> lk(d_m);
  d_max_size = rhs.d_max_size;
  d_data = rhs.d_data;
  d_data_drop_count = rhs.d_data_drop_count;
  
  return *this;
}

//----------------------------------------------------------------------//
template <class T>
ThreadPipe<T>::ThreadPipe(ThreadPipe&& rhs)
{
  *this = std::move(rhs);
}

//----------------------------------------------------------------------//
template <class T>
ThreadPipe<T>& ThreadPipe<T>::operator=(ThreadPipe&& rhs)
{
  if (this == &rhs)
    {
      return *this;
    }
  
  std::lock_guard<std::mutex> lk_rhs(rhs.d_m);
  std::lock_guard<std::mutex> lk(d_m);
  d_max_size = rhs.d_max_size;
  d_data_drop_count = rhs.d_data_drop_count;
  d_data = std::move(rhs.d_data);
  
  return *this;
}

//----------------------------------------------------------------------//
template <class T>
bool ThreadPipe<T>::empty() const
{
  std::lock_guard<std::mutex> lk(d_m);
  return d_data.empty();
}

//----------------------------------------------------------------------//
template <class T>
void ThreadPipe<T>::pushBack(const T& val)
{
  std::lock_guard<std::mutex> lk(d_m);
  if (d_data.size() > d_max_size)
    {
      ++d_data_drop_count;
      return;
    }
  d_data.push_back(val);
}

//----------------------------------------------------------------------//
template <class T>
bool ThreadPipe<T>::tryPopFront(T& val)
{
  if (empty())
    {
      return false;
    }

  std::lock_guard<std::mutex> lk(d_m);
  val = std::move(d_data.front());
  d_data.pop_front();

  return true;
}

//----------------------------------------------------------------------//
template <class T>
unsigned ThreadPipe<T>::dataDropCount() const
{
  std::lock_guard<std::mutex> lk(d_m);
  return d_data_drop_count;
}

//----------------------------------------------------------------------//
template <class T>
template <class... Args>
void ThreadPipe<T>::emplaceBack(Args&&... args)
{
  std::lock_guard<std::mutex> lk(d_m);
  if (d_data.size() > d_max_size)
    {
      ++d_data_drop_count;
      return;
    }
  d_data.emplace_back(std::forward<Args>(args)...);
}
