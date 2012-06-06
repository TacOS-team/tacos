#ifndef __STL_SET_H__
#define __STL_SET_H__

#include <vector>
#include <utility>

namespace std {

template < class T >
class set {
 public:
  typedef T value_t;
  typedef vector< value_t > data_container_t;
  typedef value_t * iterator;
  typedef _reverse_iterator<iterator, value_t> reverse_iterator;

 protected:
  data_container_t data;
 public:
  iterator find(const value_t & k) {
    for(iterator it = this->begin(); it != this->end(); ++it) {
      if(*it == k) {
        return it;
      }
    }
    return this->end();
  }

  pair<iterator, bool> insert(const value_t & v) {
    pair<iterator, bool> res;
    res.first = this->find(v);
    if(res.first == this->end()) {
        this->data.push_back(v);
        res.first = this->data.end()-1;
        res.second = true;
        *(res.first) = v;
    } else {
        res.second = false;
    }

    return res;
  }

  void insert (iterator position, const value_t & x) {
    if(position == this->end()) {
      this->data.push_back(x);
      position = this->end()-1;
    } else {
      this->data.push_back(*position);
      *position = x;
    }
    return position;
  }

  iterator insert (iterator first, iterator last) {
    for(iterator it = first; it != last; ++it) {
      this->insert(*it);
    }
  }

  /*T & operator[](const Key & k) {
    return (*((this->insert(k).first));
  }*/

  bool empty( ) const {
    return this->data.empty();
  }

  size_t size() const {
    return this->data.size();
  }

  size_t count(const T & k) {
    return this->find(k) == this->end() ? 0 : 1;
  }

  void clear() {
    this->data.clear();
  }

  void erase(iterator position) {
    *position = *(this->end()-1);
    this->data.pop_back();
  }

  void erase(iterator first, iterator last) {
    this->data.erase(first, last);
  }

  size_t erase(const T & x) {
    size_t res = 0;
    iterator pos = this->find(x);
    if(pos != this->end()) {
      this->erase(pos);
      res = 1;
    }
    return res;
  }

  /**
   * Returns a read-only (constant) iterator that points to the
   * first element in the %map. Iteration is done in ordinary
   * element order.
   * @return Read-only iterator pointing on the first element
   */
  /*const*/ iterator begin() {
    return this->data.begin();
  }
  /**
   * Returns a read-only (constant) reverse iterator that points
   * to the last element in the %map. Iteration is done in
   * reverse element order.
   * @return Read-only reverse iterator pointing on the last element
   */
  const reverse_iterator rbegin() {
    return this->data.rbegin();
  }

  /**
   * Returns a read-only (constant) iterator that points one past
   * the last element in the %map. Iteration is done in
   * ordinary element order.
   * @return Read-only iterator pointing after the last element
   */
  /*const*/ iterator end() {
    return this->data.end();
  }

  /**
   * Returns a read-only (constant) reverse iterator that points
   * to one before the first element in the %map. Iteration
   * is done in reverse element order.
   * @return Read-only reverse iterator pointing before the first element
   */
  const reverse_iterator rend() {
    return this->data.rend();
  }

};// class map

}// namespace std

#endif // __STL_SET_H__
