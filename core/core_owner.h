#ifndef CORE_OWNER_H
#define CORE_OWNER_H

#define OWNER_SPECIAL_MEMBERS(friend_name)				\
  protected:								\
  Owner& operator=(Owner&&) = delete; \ // If a move special member is declared, then the other is implicitly not-declared, and the copy constructor and operator are deleted. If the move constructor was declared here, then the default constructor would implicitly not declared.
~Owner = default; \ // must be inherited and used non-polymorphically
\
private:					\
friend class friend_name;

#endif
