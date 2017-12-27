#ifndef KN_DELETEONTIMEOUT_H
#define KN_DELETEONTIMEOUT_H

#include "kn_asiocallbacktimer.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <chrono>
#include <iostream>

namespace KERN
{
  template <class T>
  class DeleteOnTimeout
  {
  public:
    DeleteOnTimeout();
    void deletePtr(T*);
    void deletePtr(std::unique_ptr<T>&); // releases and deletes

  private:
    std::vector<T*> d_pointers; // should not be much more than one pointer buffered on most occassions
    KERN::AsioCallbackTimer d_timer = KERN::AsioCallbackTimer("KERN::DeleteOnTimeout - delete timer.");
  };

  //----------------------------------------------------------------------//
  template <class T>
    DeleteOnTimeout<T>::DeleteOnTimeout()
    {
      d_timer.setTimeoutCallback([this](){
	  for (const auto& ptr : d_pointers)
	    {
	      delete ptr;
	    }
	  d_pointers.clear();
	});
    }
  
  //----------------------------------------------------------------------//
  template <class T>
    void DeleteOnTimeout<T>::deletePtr(T* ptr)
    {
      auto it = std::find(d_pointers.begin(), d_pointers.end(), ptr);
      if (it != d_pointers.end())
	{
	  // Make sure the pointer is not already going to be deleted
	  // LOG
	  assert(false);
	  return;
	}

      bool was_empty = d_pointers.empty();
      d_pointers.push_back(ptr);

      // already deleted pointers => need to restart timer
      if (was_empty)
	{
	  d_timer.singleShot(std::chrono::seconds(0));
	}
    }

    //----------------------------------------------------------------------//
  template <class T>
    void DeleteOnTimeout<T>::deletePtr(std::unique_ptr<T>& ptr)
    {
      deletePtr(ptr.release());
    }

};

#endif
