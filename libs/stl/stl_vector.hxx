#ifndef __STL_VECTOR_HXX__
#define __STL_VECTOR_HXX__

#include <stl_vector.h>

#include <cstring>
#include <cstdlib>
#include <algorithm>

#define TEMPLATE template <typename T>
#define VECTOR std::vector<T>

TEMPLATE
VECTOR::vector () {
  m_reserve = 4;
  m_data    = (T*) malloc(m_reserve * sizeof(T)); //new T[m_reserve];
  m_size    = 0;

}// VECTOR::vector()


TEMPLATE
VECTOR::vector (size_t size, const T & value /* = T() */) {
  size = min(size, this->max_size());// TODO throw exception
  m_reserve = max(size, (size_t) 4);
  m_data    = new T [m_reserve];
  m_size    = size;
  for (size_t i = 0; i < m_size; ++i) {
    m_data[i] = value;
  }

}// VECTOR::vector()


TEMPLATE
VECTOR::~vector () {
  delete [] m_data;

}// VECTOR::~vector()


TEMPLATE
void VECTOR::push_back ( const T & x ) {
  this->insert(m_size, x);

}// VECTOR::push_back()


TEMPLATE
void VECTOR::pop_back () {
  if (m_size > 0) {
    --m_size;
  }

}// VECTOR::pop_back()


TEMPLATE
void VECTOR::insert (size_t position, const T & x) {
  this->insert(position, 1, x);

}// VECTOR::insert()


TEMPLATE
void VECTOR::insert (size_t position, size_t n, const T & x) {
  if (n >= (this->max_size() - m_size)) {
    // TODO throw exception
    return;
  }
  position = min(position, m_size);
  // On réserve de la mémoire s'il n'y en a pas assez
  if (m_size + n > m_reserve) {
    this->reserve(max(m_size + n, m_reserve * 2));
  }
  // On décale toute la mémoire
  if(m_size > 0) {
    for (size_t i = m_size - 1; i >= position; --i) {
      this->m_data[i+n] = this->m_data[i];
    }
  }
  // On écrit la valeur demandée
  for (size_t i = position; i < position + n; ++i) {
    this->m_data[i] = x;
  }

  m_size += n;

}// VECTOR::insert()

TEMPLATE
void VECTOR::insert (const iterator position, const iterator first, const iterator last) {
  size_t n = last - first;
  if (n >= (this->max_size() - m_size)) {
    // TODO throw exception
    return;
  }
  size_t pos = min((size_t) (position - this->begin()), m_size);
  // On réserve de la mémoire s'il n'y en a pas assez
  if (m_size + n > m_reserve) {
    this->reserve(max(m_size + n, m_reserve * 2));
  }
  // On décale toute la mémoire
  if(m_size > 0) {
    for (int i = m_size - 1; i >= (int)pos; --i) {
      this->m_data[i+n] = this->m_data[i];
    }
  }
  // On écrit les valeurs demandées
  iterator it = this->begin() + pos;
  for (iterator inputIt = first; inputIt != last; inputIt++, it++) {
    *it = *inputIt;
  }

  m_size += n;

}// VECTOR::insert()


TEMPLATE
void VECTOR::erase (size_t position) {
  this->erase(position, 1);

}// VECTOR::erase()


TEMPLATE
void VECTOR::erase (size_t position, size_t n) {
  if (position >= m_size) {
    return;
  }
  n = min(n, m_size - position);

  for (size_t i = position; i + n < m_size; ++i) {
    this->m_data[i] = this->m_data[i + n];
  }

  m_size -= n;

}// VECTOR::erase()


TEMPLATE
T & VECTOR::operator [] (size_t index) {
  if (index >= m_size) {
    // TODO throw exception
    //return *(new T);// Hack en attendant les exceptions
    return m_data[m_size - 1];
  }
  return m_data[index];

}// VECTOR::operator []


TEMPLATE
const T & VECTOR::operator [] (size_t index) const {
  if (index >= m_size) {
    // TODO throw exception
    //return *(new T);// Hack en attendant les exceptions
    return m_data[m_size - 1];
  }
  return m_data[index];

}// VECTOR::operator []


TEMPLATE
const T & VECTOR::front () const {
  return (*this)[0];

}// VECTOR::front ()


TEMPLATE
T & VECTOR::front () {
  return (*this)[0];

}// VECTOR::front ()


TEMPLATE
const T & VECTOR::back () const {
  return(*this)[m_size-1];

}// VECTOR::back ()


TEMPLATE
T & VECTOR::back () {
  return(*this)[m_size-1];

}// VECTOR::back ()


TEMPLATE
const T & VECTOR::at (size_t index) const {
  return (*this)[index];

}// VECTOR::at ()


TEMPLATE
T & VECTOR::at (size_t index) {
  return (*this)[index];

}// VECTOR::at ()


TEMPLATE
size_t VECTOR::size() const {
  return m_size;

}// VECTOR::size()


TEMPLATE
bool VECTOR::empty() const {
  return m_size == 0;

}// VECTOR::empty()


TEMPLATE
void VECTOR::reserve(size_t reserve) {
  T * old    = m_data;
  m_reserve  = min(reserve, this->max_size());
  m_data     = (T*) malloc(m_reserve * sizeof(T));//new T [m_reserve];
  m_size     = min(reserve, m_size);
  memcpy(m_data, old, m_size * sizeof(T));
  delete [] old;

}// VECTOR::reserve()


TEMPLATE
size_t VECTOR::max_size() const {
  return size_t(-1) / sizeof(T);

}// VECTOR::max_size()

#endif// __STL_VECTOR_HXX__
