#ifndef __STL_ALGO_H__
#define __STL_ALGO_H__

#include <cstdlib>


template <class T>
void swap(T & x, T & y) {
  T tmp = x;
  x = y;
  y = tmp;
}

template <class Iterator>
static Iterator partition(Iterator begin, Iterator end) {
  Iterator pivot    = end-1;
  Iterator posPivot = begin;
  for (Iterator current = begin; current != end; ++current) {
    if (*current < *pivot) {
      swap(*current, *posPivot);
      ++posPivot;
    }
  }
  swap(*pivot, *posPivot);

  return posPivot;
}


template <class Iterator>
static void sortRec (Iterator begin, Iterator end) {
  if (begin != end) {
    Iterator middle = partition(begin, end);
    sortRec(begin, middle);
    if (end != middle) {
      sortRec(middle + 1, end);
    }
  }
}

template <class Iterator>
void sort (Iterator first, Iterator last) {
  if (first >= last) return;
  sortRec(first, last);
}


#endif// __STL_ALGO_H__
