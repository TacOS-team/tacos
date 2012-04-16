#ifndef __STL_VECTOR_H__
#define __STL_VECTOR_H__

#include <types.h>
#include <cstdio>

namespace std
{
  template <typename T>
  class vector
  {
   public:
    class iterator {
     protected:
      size_t index;
      vector<T> * vect;
     public:
      iterator() { this->index = 0; this->vect = (vector<T> *)NULL; }
      iterator(vector<T> * vect, size_t index) { this->vect = vect; this->index = index; }
      size_t getIndex() const { return this->index; }
      bool operator == (const iterator & it) {
        if (this->vect == it.vect && this->index == it.index) {
          return true;
        }
        return false;
      }
      bool operator != (const iterator & it) {
        return !(*this == it);
      }
      iterator operator ++ (int) {
        iterator res = *this;
        ++this->index;
        return res;
      }
      iterator operator -- (int) {
        iterator res = *this;
        --this->index;
        return res;
      }
      iterator operator + (int nb) const {
        iterator res (this->vect, min(this->index + nb, vect->size()));
        return res;
      }
      T & operator * () {
        return (*this->vect)[this->index];
      }
    };
    
   protected:
    T *     m_data;
    size_t  m_size;
    size_t  m_reserve;

   public:
    vector(size_t size = 0, const T & value = T());
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
    }

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
    
    const iterator begin() {
      return iterator(this, 0);
    }
    const iterator end() {
      return iterator(this, this->m_size);
    }
    
    void clear() {
      this->m_size = 0;
    }
    iterator erase (const iterator position ) {
      return this->erase(position, position);
    }
    iterator erase (const iterator first, const iterator last ) {
      if (last.getIndex() < first.getIndex()) return first;// TODO Exception
      size_t nb = last.getIndex() - first.getIndex() + 1;
      if (this->m_size < nb) return first;// TODO Exception
      for (size_t i = last.getIndex() + 1; i < this->m_size; ++i) {
        this->m_data[i-nb] = this->m_data[i];
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

  };// class vector
  
}// namespace std

#include <stl_vector.hxx>

#endif// __STL_VECTOR_H__
