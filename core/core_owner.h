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
#endif
