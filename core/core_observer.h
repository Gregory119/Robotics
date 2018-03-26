#ifndef CORE_OBSERVER_H
#define CORE_OBSERVER_H

#include "core_specialmembers.h"

#define OBSERVER_SPECIAL_MEMBERS(friend_name)				\
  protected:								\
  Observer() = default;							\
  DEFAULT_COPYABLE(Observer);						\
  DEFAULT_MOVEABLE(Observer);						\
  ~Observer() = default;						\
private:								\
 friend class friend_name


#define ATTACH_OBSERVER()			\
  bool attachObserver(Observer* ob)		\
  {						\
    if (ob == nullptr)				\
      {						\
	assert(false);				\
	return false;				\
      }						\
    if (std::find(d_observers.begin(),		\
		  d_observers.end(),		\
		  ob) != nullptr)		\
      {						\
	assert(false);				\
	return false;				\
      }						\
    d_observers.push_back(&ob);			\
    return true;				\
  }						

#define CLEAR_OBSERVERS()			\
  void clearObservers()\
  {						\
    d_observers.clear();			\
  }

#endif
