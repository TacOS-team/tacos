#ifndef __STL_DEQUE_H__
#define __STL_DEQUE_H__

#include <types.h>
#include <iterator>

template < class T >
class deque {
 protected:
  class dequeNode {
   public:
    dequeNode *prev;
    dequeNode *next;
    T val;

    dequeNode(const T & elem) : val(elem) {
      this->prev = (dequeNode*)NULL;
      this->next = (dequeNode*)NULL;
    }
  };
  dequeNode *m_first;
  dequeNode *m_end;
  size_t m_size;

  void init (size_t size, const T& value = T()) {
    // Always a default elem at the end
    this->m_first = new dequeNode(T());
    this->m_end  = this->m_first;
    this->m_size  = 0;
    for (size_t i = 0; i < size; ++i) {
      this->push_front(value);
    }
  }

  dequeNode * nodeAt (size_t index) {
    dequeNode * res = this->m_first;
    if (index >= this->m_size) {
      // TODO exception
      return res;
    }
    for (size_t i = 0; i < index; ++i) {
      res = res->next;
    }
    return res;
  }

  void insertBefore(dequeNode * node, const T& value = T()) {
    dequeNode *newNode = new dequeNode(value);
    if (node->prev != NULL) {
      node->prev->next = newNode;
      newNode->prev = node->prev;
      newNode->next = node;
      this->m_end->prev = newNode;
    } else {// First element
      this->m_first->prev = newNode;
      newNode->next = this->m_first;
      this->m_first = newNode;
    }
    ++this->m_size;
  }

  void deleteNode(dequeNode * node) {
    node->next->prev = node->prev;
    if (node == this->m_first) {
      this->m_first = node->next;
    } else {
      node->prev->next = node->next;
    }
    delete node;
    --this->m_size;
  }

 public:
  class iterator {
    friend class deque;
   protected:
    dequeNode *node;
   public:
    iterator() { this->node = (dequeNode*) NULL; }
    iterator(dequeNode *node) { this->node = node; }
    bool operator == (const iterator & it) {
      if (this->node == it.node) {
        return true;
      }
      return false;
    }
    bool operator != (const iterator & it) {
      return !(*this == it);
    }
    T & operator * () {
      return this->node->val;
    }
    iterator operator ++ (int) {
      iterator res = *this;
      ++(*this);
      return res;
    }
    iterator operator ++ () {
      if (this->node != NULL) {
        this->node = this->node->next;
      }// TODO else exception
      return *this;
    }
    iterator operator -- (int) {
      iterator res = *this;
      --(*this);
      return res;
    }
    iterator operator -- () {
      if (this->node != NULL && this->node->prev != NULL) {
        this->node = this->node->prev;
      }// TODO else exception
      return *this;
    }
    iterator operator + (int nb) const {
      iterator res = *this;
      if (res.node == NULL) return res; // TODO throw exception
      for (int i = 0; i < nb; ++i) {
        if (res.node->next != NULL) {
          res.node = res.node->next;
        }// TODO else exception
      }
      return res;
    }
    iterator & operator += (int nb) {
      *this = *this + nb;
      return *this;
    }
    iterator operator - (int nb) const {
      iterator res = *this;
      if (res.node == NULL) return res; // TODO throw exception
      for (int i = 0; i < nb; ++i) {
        if (res.node->prev != NULL) {
          res.node = res.node->prev;
        }// TODO else exception
      }
      return res;
    }
    iterator & operator -= (int nb) {
      *this = *this - nb;
      return *this;
    }
  };

  typedef _reverse_iterator<iterator, T> reverse_iterator;

  deque() {
    this->init(0);
  }

  deque(size_t size, const T& value = T()) {
    this->init(size, value);
  }

  deque(const deque<T> & x) {
    *this = x;
  }

  ~deque() {
    this->clear();
    delete this->m_end;
  }

  void clear() {
    for (dequeNode *p = this->m_first; p!= this->m_end; p = p->next) {
      delete p;
    }
    this->m_first = this->m_end;
    this->m_size = 0;
  }

  deque<T> & operator = (const deque<T> & x) {
    this->clear();
    for (iterator it = x->begin(); it != x->end(); ++it) {
      this->push_back(*it);
    }
  }

  void push_back(const T & elem) {
    // TODO si size max exception
    this->insertBefore(this->m_end, elem);
  }

  void push_front(const T & elem) {
    // TODO si size max exception
    this->insertBefore(this->m_first, elem);
  }

  iterator begin() {
    return iterator(this->m_first);
  }

  reverse_iterator rbegin() {
    return reverse_iterator(iterator(this->m_end));
  }

  iterator end() {
    return iterator(this->m_end);
  }

  reverse_iterator rend() {
    return reverse_iterator(iterator(this->m_first));
  }

  size_t size() {
    return this->m_size;
  }

  T & front() {
    // TODO exception if empty
    return this->m_first->val;
  }

  T & back() {
    // TODO exception if empty
    if (this->m_end->prev == NULL) {
      // Hack en attendant les exceptions
      return this->m_end->val;
    }
    return this->m_end->prev->val;
  }

  T & at (size_t index) {
    return this->nodeAt(index)->val;
  }

  T & operator [] (size_t index) {
    return this->nodeAt(index)->val;
  }

  void resize (size_t sz, const T & c = T()) {
    if (sz == 0) {
      this->clear();
    } else if (sz >= this->m_size) {
      while (this->m_size < sz) {
        this->insertBefore(this->m_end, c);
      }
    } else {
      this->erase(iterator(this->nodeAt(sz)), this->end());
    }
    this->m_size = sz;
  }

  void pop_back() {
    if (this->m_size == 0) return; // TODO exception
    this->deleteNode(this->m_end->prev);
  }

  void pop_front() {
    if (this->m_size == 0) return; // TODO exception
    this->deleteNode(this->m_first);
  }

  iterator insert (iterator position, const T& x) {
    this->insertBefore(position.node, x);
    return iterator(position.node->prev);
  }

  void insert ( iterator position, size_t n, const T& x ) {
    for (size_t i = 0; i < n; ++i) {
      this->insertBefore(position.node, x);
    }
  }

  iterator erase (iterator position) {
    if (position == this->end() && this->m_size > 0) {
      return this->erase(position - 1, position);
    }
    return this->erase(position, position+1);
  }

  iterator erase (iterator first, iterator last) {
    // TODO last before first ?
    dequeNode * current = first.node;
    if (first == this->begin()) {
      this->m_first = last.node;
      this->m_first->prev = (dequeNode*)NULL;
    } else {
      first.node->prev->next = last.node;
      last.node->prev = first.node->prev;
    }
    while (current != last.node) {
      dequeNode * toDelete = current;
      current = current->next;
      delete toDelete;
    }
    return last;
  }

  bool empty() {
    return this->m_size == 0;
  }

  void swap (deque<T>& dqe) {
    dequeNode * first = this->m_first;
    dequeNode * end   = this->m_end;
    size_t      size  = this->m_size;

    this->m_first = dqe.m_first;
    this->m_end   = dqe.m_end;
    this->m_size  = dqe.m_size;

    dqe.m_first = first;
    dqe.m_end   = end;
    dqe.m_size  = size;
  }
};

#endif// __STL_DEQUE_H__
