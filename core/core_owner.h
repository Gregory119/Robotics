#ifndef CORE_OWNER_H
#define CORE_OWNER_H

// If a move special member is declared, then the other is implicitly not-declared, and the copy constructor and operator are deleted. If the move constructor was declared here, then the default constructor would be implicitly not declared.
// The private destructor is used to ensure inheritance and disable non-polymorphic use.
#define OWNER_SPECIAL_MEMBERS(friend_name)				\
  protected:								\
  Owner() = default;							\
  Owner& operator=(Owner&&) = delete;					\
  ~Owner() = default;							\
private:								\
 friend class friend_name

#define OWNER_ERROR_FUNC(friend_name)		\
  virtual void handleError(friend_name*, Error, const std::string& msg) = 0

#include <functional>
#include <cassert>

namespace CORE
{
  template <class T>
  class Owner final
  {
  public:
    Owner(T* o)
      {
	if (o == nullptr)
	  {
	    // LOG
	    assert(false);
	    return;
	  }
      }
    
    Owner& operator=(T* o)
      {
	if (o == nullptr)
	  {
	    // LOG
	    assert(false);
	    d_is_disabled = true;
	    return;
	  }

	d_is_disabled = false;
      }
    Owner(Owner&&) = delete; // implicitly not moveable or copyable

    template <class F, class... Args>
      void call(F&& f, Args&&... args)
    {
      if (!d_is_disabled)
	{
	  auto func = std::bind(f, d_owner, std::forward<Args>(args)...);
	  func();
	}
    }
    
  protected:
    Owner& operator=(Owner&&) = delete; // implicitly not declare other move members, and delete copy members
    ~Owner() = default; // non-polymorphic

  private:
    T* d_owner = nullptr;
    bool d_is_disabled = false;
  };
};

#endif
