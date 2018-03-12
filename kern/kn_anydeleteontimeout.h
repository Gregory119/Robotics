#ifndef KN_ANYDELETEONTIMEOUT_H
#define KN_ANYDELETEONTIMEOUT_H

#include "kn_asiocallbacktimer.h"

#include <algorithm>
#include <memory>
#include <vector>
#include <chrono>
#include <iostream>

namespace KERN
{
  class BaseDeleter
  {
  public:
    virtual ~BaseDeleter() = default;
    // If a move special member is declared, then the other is implicitly not-declared, and the copy constructor and operator are deleted.
    BaseDeleter(BaseDeleter&&) = default;

    virtual void* getPointer() const = 0;
    
  protected:
    BaseDeleter() = default;
  };
  
  template <class T>
    class Deleter final : public BaseDeleter
  {
  public:
    explicit Deleter(T* p)
      : d_pointer(p)
      {}
    
    ~Deleter()
      {
	if (d_pointer != nullptr)
	  {
	    delete d_pointer;
	  }
      }
    // If a move special member is declared, then the other is implicitly not-declared, and the copy constructor and operator are deleted.
    Deleter& operator=(Deleter&&) = delete;

    void* getPointer() const override { return d_pointer; }
    
  private:
    T* d_pointer = nullptr;
  };
  
  class AnyDeleteOnTimeout
  {
  public:
    AnyDeleteOnTimeout();

    template <class T>
    void deletePtr(T*);

    template <class T>
    void deletePtr(std::unique_ptr<T>&);

  private:
    std::vector<std::unique_ptr<BaseDeleter>> d_base_deleters; // could be a number of classes requested to be deleted
    KERN::AsioCallbackTimer d_timer = KERN::AsioCallbackTimer("KERN::AnyDeleteOnTimeout - delete timer.");
  };

  //----------------------------------------------------------------------//
  template <class T>
    void AnyDeleteOnTimeout::deletePtr(T* ptr)
    {
      // already deleted previous classes => need to restart timer to delete new list
      if (d_base_deleters.empty())
	{
	  d_timer.singleShot(std::chrono::seconds(0));
	}
      
      // Check that the pointer is not already going to be deleted
      for (const auto& deleter : d_base_deleters)
	{
	  if (deleter->getPointer() == ptr)
	    {
	      // LOG
	      assert(false);
	      return;
	    }
	}

      d_base_deleters.emplace_back(new Deleter<T>(ptr));
    }

  //----------------------------------------------------------------------//
  template <class T>
    void AnyDeleteOnTimeout::deletePtr(std::unique_ptr<T>& ptr)
    {
      deletePtr(ptr.release());
    }
};

#endif
