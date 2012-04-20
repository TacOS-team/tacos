/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
 * Copyright (c) 1996,1997
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

#ifndef __STL_PAIR_H__
#define __STL_PAIR_H__

namespace std {
  template <class _T1, class _T2>
  struct pair {
    typedef _T1 first_type; /**< @c first_type is the first bound type */
    typedef _T2 second_type; /**< @c second_type is the second bound type */

    _T1 first; /**< @c first is a copy of the first object */
    _T2 second; /**< @c second is a copy of the second object */

    /**
     * The default constructor creates @c first and @c second using their
     * respective default constructors.
     */
    pair() : first(_T1()), second(_T2()) {}

    /**
     * Two objects may be passed to a @c pair constructor to be copied.
     * @param __a First element.
     * @param __b Second element.
     */
    pair(const _T1& __a, const _T2& __b) : first(__a), second(__b) {}

    /**
     * There is also a templated copy ctor for the @c pair class itself.
     * @param __p Source pair.
     */
    template <class _U1, class _U2>
    pair(const pair<_U1, _U2>& __p) : first(__p.first), second(__p.second) {}
  };

  /**
   * Two pairs of the same type are equal iff their members are equal.
   * @return true if the pairs are equal
   */
  template <class _T1, class _T2>
  inline bool operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
    return __x.first == __y.first && __x.second == __y.second;
  }

  /**
   * Uses @c operator== to find the result.
   * @return true if the pairs are not equal
   */
  template <class _T1, class _T2>
  inline bool operator!=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
    return !(__x == __y);
  }

  /**
   * The comparison is done on the first member first, and then on the second.
   * @return true if @a __x < @a __y
   */
  template <class _T1, class _T2>
  inline bool operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
    return __x.first < __y.first ||
           (!(__y.first < __x.first) && __x.second < __y.second);
  }

  /**
   * Uses @c operator< to find the result.
   * @return true if @a __x > @a __y
   */
  template <class _T1, class _T2>
  inline bool operator>(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
    return __y < __x;
  }

  /**
   * Uses @c operator< to find the result.
   * @return true if @a __x <= @a __y
   */
  template <class _T1, class _T2>
  inline bool operator<=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
    return !(__y < __x);
  }

  /**
   * Uses @c operator< to find the result.
   * @return true if @a __x >= @a __y
   */
  template <class _T1, class _T2>
  inline bool operator>=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y) {
    return !(__x < __y);
  }

  /**
   * A convenience wrapper for creating a pair from two objects.
   * @param __x The first object.
   * @param __y The second object.
   * @return A newly-constructed pair<> object of the appropriate type.
   */
  template <class _T1, class _T2>
  inline pair<_T1, _T2> make_pair(const _T1& __x, const _T2& __y) {
    return pair<_T1, _T2>(__x, __y);
  }

}

#endif // __STL_PAIR_H__
