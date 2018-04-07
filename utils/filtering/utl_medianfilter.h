#ifndef UTIL_MEDIANFILTER_H
#define UTIL_MEDIANFILTER_H

namespace UTIL
{
  template <class T>
  class MedianFilter final
  {
  public:
    MedianFilter(size_t size=3);

    void setSize(size_t size);

    template <class T1>
    void pushBack(T1&&);
    
    T getMedian();

  private:
    void reduceToSize();
    
  private:
    size_t d_size = 0;
    std::vector<T> d_data;
    std::vector<T> d_sorted_data;
  };

  //----------------------------------------------------------------------//
  template <class T>
    MedianFilter<T>::MedianFilter(size_t size)
    : d_size(size)
    {
      if (size < 2)
	{
	  assert(false);
	  return;
	}
      d_data.reserve(d_size);
    }

  //----------------------------------------------------------------------//
  template <class T>
    void MedianFilter<T>::setSize(size_t size)
    {
      if (size < 2)
	{
	  assert(false);
	  return;
	}
      d_size = size;
      reduceToSize();
      d_data.reserve(size); // will only reallocate if the desired size is greater than the current capacity
    }
    
  //----------------------------------------------------------------------//
  template <class T>
    template <class T1>
    void MedianFilter<T>::pushBack(T1&& val)
    {
      d_data.push_back(std::forward<T1>(val));
      reduceToSize();
    }
    
  //----------------------------------------------------------------------//
  template <class T>
    T MedianFilter<T>::getMedian()
    {
      if (d_data.size() < d_size)
	{
	  return d_data.back();
	}
      
      d_sorted_data = d_data;
      std::sort(d_sorted_data.begin(), d_sorted_data.end());

      if ((d_size % 2) == 1) // odd
	{
	  return d_sorted_data[d_size/2];
	}
      
      // even
      size_t index_high = d_size/2;
      return (d_sorted_data[index_high-1]+d_sorted_data[index_high])/2;
    }

  //----------------------------------------------------------------------//
  template <class T>
    void MedianFilter<T>::reduceToSize()
    {
      int diff = static_cast<int>(d_data.size()) - static_cast<int>(d_size);
      if (diff > 0)
	{
	  d_data.erase(d_data.begin(), d_data.begin()+diff);
	  assert(d_data.size() == d_size);
	}
    }
};

#endif
