#ifndef __STL_STACK_H__
#define __STL_STACK_H__

#include <deque>

namespace std {

  template < class T, class Container = deque<T> >
  class stack {
   protected:
    Container container; /**< Container. */

   public:
    /**
     * Default constructor creates no elements.
     * @param ctnr Container object used to store elements
     */
    stack(const Container& ctnr = Container());

    /**
     * Returns true if the %stack is empty.
     * @return true if the %stack is empty
     */
    bool empty();

    /** 
     * Returns the number of elements in the %stack.
     * @return The number of elements
     */
    size_t size();

    /**
     * Removes the first element of the %stack.
     */
    void pop();

    /**
     * Adds data to the top of the %stack.
     * @param elem Data to be added.
     */
    void push(const T & elem);

    /**
     * Returns a read/write reference to the data at the first
     * element of the %stack.
     * @return Read-write reference to the first element
     */
    T & top();
  };

}

#include <stl_stack.hxx>

#endif // __STL_STACK_H__
