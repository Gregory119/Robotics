#ifndef CORE_OWNER_H
#define CORE_OWNER_H

// A declared move special member implicitly does not declared the other move special member, and the copy constructor and operator are deleted.
// A declared move constructor implicitly does not declare the default constructor.
// A protected destructor ensures inheritance and disables polymorphic use.
#define OWNER_SPECIAL_MEMBERS(friend_name)				\
  protected:								\
  Owner() = default;							\
  ~Owner() = default;							\
  Owner(const Owner&) = delete;						\
  Owner& operator=(const Owner&) = delete;				\
  Owner(Owner&&) = default;						\
  Owner& operator=(Owner&&) = default;					\
private:								\
 friend class friend_name

#define OWNER_ERROR_FUNC(friend_name)		\
  virtual void handleError(friend_name*, Error, const std::string& msg) = 0

#define SET_OWNER()		\
  void setOwner(Owner* o) { d_owner = o; }

#include <cassert>
#include <functional>

namespace CORE
{
  template <class T>
  class Owner final
  {
  public:
    Owner(T* o);
    Owner& operator=(T* o);
    Owner& operator=(const Owner&) = delete;
    Owner(const Owner&) = delete;
    Owner& operator=(Owner&& rhs);
    Owner(Owner&&);

    template <class F, class... Args>
      void call(F&& f, Args&&... args);
    
  private:
    T* d_owner = nullptr;
    bool d_is_disabled = false;
  };

  template <class T>
    Owner<T>::Owner(T* o)
    : d_owner(o)
    {
      if (o == nullptr)
	{
	  // LOG
	  assert(false);
	  d_is_disabled = true;
	  return;
	}
    }

  template <class T>
    Owner<T>& Owner<T>::operator=(T* o)
    {
      if (o == nullptr)
	{
	  // LOG
	  assert(false);
	  d_is_disabled = true;
	  return *this;
	}
      
      d_is_disabled = false;
      d_owner = o;
    }

  template <class T>
  Owner<T>& Owner<T>::operator=(Owner&& rhs)
    {
      if (this == &rhs)
	{
	  return *this;
	}

      if (rhs.d_owner == nullptr)
	{
	  // LOG
	  assert(false);
	  d_is_disabled = true;
	  return *this;
	}
      
      d_owner = rhs.d_owner;
      rhs.d_is_disabled = true;
      return *this;
    }

  template <class T>
    Owner<T>::Owner(Owner&& rhs)
    {
      *this = rhs;
    }

  template <class T>
    template <class F, class... Args>
    void Owner<T>::call(F&& f, Args&&... args)
  {
    if (!d_is_disabled)
      {
	try
	  {
	    auto func = std::bind(f, d_owner, std::forward<Args>(args)...);
	    func();
	  }
	catch (...)
	  {
	    // LOG
	    assert(false);
	    return;
	  }
      }
  }
};

#endif
