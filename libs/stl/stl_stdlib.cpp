#include <stl_stdlib.h>


void * operator new(unsigned int size)
{
    return malloc(size);
}

void * operator new[](unsigned int size)
{
    return malloc(size);
}

void operator delete(void * p)
{
    free(p);
}

void operator delete[](void * p)
{
    free(p);
}

void operator delete(void * p, unsigned int)
{
    free(p);
}

void operator delete[](void * p, unsigned int)
{
    free(p);
}

inline void *operator new(unsigned int, void *p)     throw() { return p; }
inline void *operator new[](unsigned int, void *p)   throw() { return p; }
inline void  operator delete  (void *, void *) throw() { }
inline void  operator delete[](void *, void *) throw() { }
