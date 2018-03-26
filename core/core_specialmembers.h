#ifndef CORE_SPECIALMEMBERS_H
#define CORE_SPECIALMEMBERS_H

#define NOT_COPYABLE(Name)						\
  Name(const Name&) = delete;						\
  Name& operator=(const Name&) = delete

#define DEFAULT_COPYABLE(Name)						\
  Name(const Name&) = default;						\
  Name& operator=(const Name&) = default

#define NOT_MOVEABLE(Name)						\
  Name(Name&&) = delete;						\
  Name& operator=(Name&&) = delete

#define DEFAULT_MOVEABLE(Name)						\
  Name(Name&&) = default;						\
  Name& operator=(Name&&) = default

#endif
