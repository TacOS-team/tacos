#ifndef __STL_VECTOR_H__
#define __STL_VECTOR_H__

#include <types.h>

namespace std
{
  template <typename T>
  class vector
  {
   protected:
    T *     m_data;
    size_t  m_size;
    size_t  m_reserve;

   public:
    vector(size_t reserve = 8);

    void push_back ( const T & x );

    const T & operator [] (size_t index) const;
          T & operator [] (size_t index);

    size_t size() const;

  };// class vector
  
}// namespace std

#include <stl_vector.hxx>

#endif// __STL_VECTOR_H__
