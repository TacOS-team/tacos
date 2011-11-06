#ifndef __STL_ALGOBASE_H__
#define __STL_ALGOBASE_H__

template <class T>
inline const T & min(const T & a, const T & b) {
  return b < a ? b : a;
}

template <class T>
inline const T & max(const T & a, const T & b){
  return  a < b ? b : a;
}

#endif// __STL_ALGOBASE_H__
