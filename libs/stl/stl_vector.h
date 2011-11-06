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
    vector(size_t size = 0, const T & value = T());
    ~vector();

    void push_back (const T & x);
    void pop_back ();

    void insert (size_t position, const T & x);
    void insert (size_t position, size_t n, const T & x);

    void erase (size_t position);
    void erase (size_t position, size_t n);

    const T & operator [] (size_t index) const;
          T & operator [] (size_t index);

    const T & front () const;
          T & front ();

    const T & back () const;
          T & back ();

    const T & at (size_t index) const;
          T & at (size_t index);

    size_t size() const;
    void reserve(size_t reserve);

    size_t max_size() const;

  };// class vector
  
}// namespace std

#include <stl_vector.hxx>

#endif// __STL_VECTOR_H__
