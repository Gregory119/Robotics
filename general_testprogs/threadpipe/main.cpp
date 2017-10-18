#include <iostream>
#include <mutex>
#include <thread>
#include <deque>
#include <chrono>

template <class T> class ThreadPipe
{
public:
  ThreadPipe() = default;
  ThreadPipe(int max_size) : d_max_size(max_size) {}
  ThreadPipe(const ThreadPipe<T>& rhs) { *this = rhs; }
  ThreadPipe<T>& operator=(const ThreadPipe<T>& rhs)
  {
    std::lock_guard<std::mutex> lk(rhs.d_m);
    d_m = rhs.d_m;
    d_max_size = rhs.d_max_size;
    d_data = rhs.d_data;

    return *this;
  }

  bool empty() const
  {
    std::lock_guard<std::mutex> lk(d_m);
    return d_data.empty();
  }

  void pushBack(const T& val)
  {
    std::lock_guard<std::mutex> lk(d_m);
    d_data.push_back(val);
  }

  bool tryPopFront(T& val)
  {
    if (empty())
      {
	return false;
      }

    std::lock_guard<std::mutex> lk(d_m);
    val = d_data.front();
    d_data.pop_front();

    return true;
  }

private:
  mutable std::mutex d_m; // mutable for use in "empty() const"
  int d_max_size = 100;   // Could drop data if data is being processed slower
  // than it is coming in

  std::deque<T> d_data;
};

int g_num = 0;

// use future and promise to end thread function
void thread_funcA(ThreadPipe<int>& pipe)
{
  while (1)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      pipe.pushBack(g_num);
      ++g_num;
    }
}



int main(int argc, char** argv)
{
  ThreadPipe<int> pipe;
  std::thread thread_a(thread_funcA, std::ref(pipe));

  int temp = 0;
  while (1)
    {
      while (!pipe.empty())
	{
	  if (pipe.tryPopFront(temp))
	    {
	      std::cout << "pipe value = " << temp << std::endl;
	    }
	}
    }

  thread_a.join();

  return 0;
}
