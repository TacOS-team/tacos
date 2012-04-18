#ifndef __STL_ITERATOR_H__
#define __STL_ITERATOR_H__

namespace std {

  // Serait bien d'arriver à demander des templates
  template <class Iterator, typename T>
  class _reverse_iterator {
   protected:
    Iterator it;

   public:
    _reverse_iterator(const Iterator & iter) : it(iter) { }
    bool operator == (const _reverse_iterator<Iterator, T> & it) {
      if (this->it == it.it) {
        return true;
      }
      return false;
    }
    bool operator != (const _reverse_iterator<Iterator, T> & it) {
      return !(*this == it);
    }
    Iterator base() {
      return this->it;
    }
    T & operator * () {
      Iterator res = this->it;
      --res;
      return (*res);
    }
    // TODO gérer les effets de bords
    _reverse_iterator<Iterator, T> operator ++ (int) {
      return _reverse_iterator<Iterator, T>(this->it--);
    }
    _reverse_iterator<Iterator, T> operator ++ () {
      return _reverse_iterator<Iterator, T>(--this->it);
    }
    _reverse_iterator<Iterator, T> operator -- (int) {
      return _reverse_iterator<Iterator, T>(this->it++);
    }
    _reverse_iterator<Iterator, T> operator -- () {
      return _reverse_iterator<Iterator, T>(++this->it);
    }
    _reverse_iterator<Iterator, T> operator + (int nb) const {
      _reverse_iterator<Iterator, T> res (this->it);
      res.it -= nb;
      return res;
    }
    _reverse_iterator<Iterator, T> & operator += (int nb) {
      *this = *this + nb;
      return *this;
    }
    _reverse_iterator<Iterator, T> operator - (int nb) const {
      _reverse_iterator<Iterator, T> res (this->it);
      res.it += nb;
      return res;
    }
    _reverse_iterator<Iterator, T> & operator -= (int nb) {
      *this = *this - nb;
      return *this;
    }
  };
}


#endif// __STL_ITERATOR_H__
