#ifndef __STL_VECTOR_HXX__
#define __STL_VECTOR_HXX__

#include <stl_vector.h>

#include <cstdio>
#include <stdlib>

#include <cstring>

#define TEMPLATE template <typename T>
#define VECTOR std::vector<T>


TEMPLATE
VECTOR::vector (size_t reserve /* = 8*/ )
{
  m_reserve = reserve;
  m_size    = 0;
  m_data    = new T [reserve];

}// VECTOR::vector()


TEMPLATE
void VECTOR::push_back ( const T & x )
{
  if (m_size >= m_reserve)
  {
    T * old    = m_data;
    m_reserve *= 2;
    m_data     = new T [m_reserve];
    memcpy(m_data, old, m_size * sizeof(T));
  }
  m_data[m_size] = x;
  ++m_size;

}// VECTOR::push_back()


TEMPLATE
T & VECTOR::operator [] (size_t index)
{
  return m_data[index];

}// VECTOR::operator []


TEMPLATE
const T & VECTOR::operator [] (size_t index) const
{
  return m_data[index];

}// VECTOR::operator []


TEMPLATE
size_t VECTOR::size() const
{
  return m_size;

}// VECTOR::size()


#endif// __STL_VECTOR_HXX__









