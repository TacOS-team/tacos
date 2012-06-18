#ifndef __STL_ALGOBASE_H__
#define __STL_ALGOBASE_H__

namespace std {
  template <class T>
  inline const T & min(const T & a, const T & b) {
    return b < a ? b : a;
  }

  template <class T>
  inline const T & max(const T & a, const T & b){
    return a < b ? b : a;
  }

  template<class Iterator, class T>
  Iterator find(Iterator first, Iterator last, const T &value) {
    for (; first != last; first++) {
      if (*first == value) {
        break;
      }
    }

    return first;
  }
}

#endif// __STL_ALGOBASE_H__
