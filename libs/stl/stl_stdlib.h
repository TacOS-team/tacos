#ifndef __STL_STDLIB_H__
#define __STL_STDLIB_H__

extern "C" {
  #include <malloc.h>
  #include <sys/types.h>
}

void * operator new(unsigned int size);

void * operator new[](unsigned int size);

void operator delete(void * p);

void operator delete[](void * p);

inline void *operator new(unsigned int, void *p)     throw();
inline void *operator new[](unsigned int, void *p)   throw();
inline void  operator delete  (void *, void *) throw();
inline void  operator delete[](void *, void *) throw();


#endif// __STL_STDLIB_H__
