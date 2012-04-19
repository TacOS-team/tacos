#ifndef __STL_VECTOR_HXX__
#define __STL_VECTOR_HXX__

#include <stl_vector.h>

#include <cstring>
#include <cstdlib>
#include <algorithm>

#define TEMPLATE template <typename T>
#define VECTOR std::vector<T>


TEMPLATE
VECTOR::vector() {
  m_reserve = 4;
  m_data    = (T*) malloc(m_reserve * sizeof(T)); //new T[m_reserve];
  m_size    = 0;

}// VECTOR::vector()


TEMPLATE
VECTOR::vector(size_t size, const T & value /* = T() */) {
  size = min(size, this->max_size());// TODO throw exception
  m_reserve = max(size, (size_t) 4);
  m_data    = new T [m_reserve];
  m_size    = size;
  for (size_t i = 0; i < m_size; ++i) {
    m_data[i] = value;
  }

}// VECTOR::vector()


TEMPLATE
VECTOR::vector(const vector<T> & v) {
  *this = v;

}// VECTOR::vector()


TEMPLATE
VECTOR::~vector() {
  delete [] m_data;

}// VECTOR::~vector()


TEMPLATE
VECTOR & VECTOR::operator = (const vector<T> & v) {
  this->m_reserve = v.m_reserve;
  this->m_data    = new T [this->m_reserve];
  this->m_size    = v.m_size;
  for (size_t i = 0; i < this->m_size; ++i) {
    this->m_data[i] = v.m_data[i];
  }
  return *this;
}// VECTOR::operator = ()


TEMPLATE
void VECTOR::push_back( const T & x ) {
  this->insert(this->end(), x);

}// VECTOR::push_back()


TEMPLATE
void VECTOR::pop_back() {
  if (m_size > 0) {
    --m_size;
  }

}// VECTOR::pop_back()


TEMPLATE
void VECTOR::insert(const iterator position, const T & x) {
  this->insert(position, 1, x);

}// VECTOR::insert()


TEMPLATE
void VECTOR::insert(const iterator position, size_t n, const T & x) {
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
  for (size_t i = pos; i < pos + n; ++i) {
    this->m_data[i] = x;
  }

  m_size += n;

}// VECTOR::insert()


TEMPLATE
void VECTOR::insert(const iterator position, const iterator first, const iterator last) {
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
const T & VECTOR::front() const {
  return (*this)[0];

}// VECTOR::front()


TEMPLATE
T & VECTOR::front() {
  return (*this)[0];

}// VECTOR::front()


TEMPLATE
const T & VECTOR::back() const {
  return(*this)[m_size-1];

}// VECTOR::back()


TEMPLATE
T & VECTOR::back() {
  return(*this)[m_size-1];

}// VECTOR::back()


TEMPLATE
const T & VECTOR::at(size_t index) const {
  return (*this)[index];

}// VECTOR::at()


TEMPLATE
T & VECTOR::at(size_t index) {
  return (*this)[index];

}// VECTOR::at()


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


TEMPLATE
typename VECTOR::iterator VECTOR::begin() {
  return m_data;
}// VECTOR::begin()


TEMPLATE
const typename VECTOR::reverse_iterator VECTOR::rbegin() {
  return reverse_iterator(m_data + this->size());
}// vECTOR::rbegin()


TEMPLATE
typename VECTOR::iterator VECTOR::end() {
  return m_data + this->size();
}// VECTOR::end()


TEMPLATE
const typename VECTOR::reverse_iterator VECTOR::rend() {
  return reverse_iterator(m_data);
}// VECTOR::rend()


TEMPLATE
void VECTOR::clear() {
  this->m_size = 0;
}// VECTOR::clear()


TEMPLATE
typename VECTOR::iterator VECTOR::erase (const iterator position ) {
  return this->erase(position, position+1);
}// VECTOR::erase()


TEMPLATE
typename VECTOR::iterator VECTOR::erase (const iterator first, const iterator last ) {
  if (last <= first) return first;// TODO Exception
  size_t nb = last - first;
  if (this->m_size < nb + (size_t) (first - this->begin())) return first;// TODO Exception
  iterator from = last;
  iterator to = first;
  while (from != this->end()) {
    *to = *from;
    ++from;
    ++to;
  }
  this->m_size -= nb;
  return first;
}// VECTOR::erase()


TEMPLATE
void VECTOR::swap (vector<T> & v) {
  T *     data = this->m_data;
  size_t  size = this->m_size;
  size_t  reserve = this->m_reserve;

  this->m_data = v.m_data;
  this->m_size = v.m_size;
  this->m_reserve = v.m_reserve;

  v.m_data = data;
  v.m_size = size;
  v.m_reserve = reserve;

}// VECTOR::swap()


#endif// __STL_VECTOR_HXX__
