#ifndef __STL_VECTOR_H__
#define __STL_VECTOR_H__

#include <types.h>

namespace std
{
  template <typename T>
  class vector
  {
   protected:
    T *       data;
    uint16_t  size;

   public:
    vector(uint16_t size);

  };// class vector
  
}// namespace std

#include <stl_vector.hxx>

#endif// __STL_VECTOR_H__
