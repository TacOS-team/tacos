#ifndef __STL_VECTOR_H__
#define __STL_VECTOR_H__

#include <iterator>
#include <sys/types.h>

namespace std {
  template <typename T>
  class vector {
   public:
    typedef T *iterator;
    typedef _reverse_iterator<iterator, T> reverse_iterator;
    
   protected:
    T *     m_data;
    size_t  m_size;
    size_t  m_reserve;

   public:
    vector();
    vector(size_t size, const T & value = T());
    vector(const vector<T> & v) {
      *this = v;
    }
    ~vector();
    
    vector<T> & operator = (const vector<T> & v) {
      this->m_reserve = v.m_reserve;
      this->m_data    = new T [this->m_reserve];
      this->m_size    = v.m_size;
      for (size_t i = 0; i < this->m_size; ++i) {
        this->m_data[i] = v.m_data[i];
      }
      return *this;
    }

    void push_back (const T & x);
    void pop_back ();

    void insert (size_t position, const T & x);
    void insert (size_t position, size_t n, const T & x);
    void insert (const iterator position, const iterator first, const iterator last);

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
    bool empty() const;
    void reserve(size_t reserve);

    size_t max_size() const;
    
    iterator begin() {
      return m_data;
    }
    const reverse_iterator rbegin() {
      return reverse_iterator(m_data + this->size());
    }
    iterator end() {
      return m_data + this->size();
    }
    const reverse_iterator rend() {
      return reverse_iterator(m_data);
    }
    
    void clear() {
      this->m_size = 0;
    }
    iterator erase (const iterator position ) {
      return this->erase(position, position+1);
    }
    iterator erase (const iterator first, const iterator last ) {
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
    }
    
    void swap (vector<T> & v) {
      T *     data = this->m_data;
      size_t  size = this->m_size;
      size_t  reserve = this->m_reserve;
      
      this->m_data = v.m_data;
      this->m_size = v.m_size;
      this->m_reserve = v.m_reserve;
      
      v.m_data = data;
      v.m_size = size;
      v.m_reserve = reserve;
      
    }

    bool empty() {
      return this->m_size == 0;
    }

  };// class vector
  
}// namespace std

#include <stl_vector.hxx>

#endif// __STL_VECTOR_H__
