#ifndef __STL_VECTOR_HXX__
#define __STL_VECTOR_HXX__

#include <stl_vector.h>

#include <cstdio>

#define TEMPLATE template <typename T>
#define VECTOR std::vector<T>


TEMPLATE
VECTOR::vector (uint16_t size)
{
  this->size = size;
  //this->data = new T [size];

}// VECTOR::vector()


#endif// __STL_VECTOR_HXX__
