#ifndef __STL_QUEUE_H__
#define __STL_QUEUE_H__

#include <queue>
#include <deque>

namespace std {

  template < class T, class Container = deque<T> >
  class queue {
   protected:
    Container container; /**< Container. */

   public:
    /**
     * Default constructor creates no elements.
     * @param ctnr Container object used to store elements
     */
    queue(const Container& ctnr = Container());

    /**
     * Returns true if the %queue is empty.
     * @return true if the %queue is empty
     */
    bool empty();

    /**
     * Returns the number of elements in the %queue.
     * @return The number of elements
     */
    size_t size();

    /**
     * Removes the first element of the %queue.
     */
    void pop();

    /**
     * Adds data to the end of the %queue.
     * @param elem Data to be added.
     */
    void push(const T & elem);

    /**
     * Returns a read/write reference to the data at the first
     * element of the %queue.
     * @return Read-write reference to the first element
     */
    T & front();

    /**
     * Returns a read/write reference to the data at the last
     * element of the %queue.
     * @return Read-write reference to the last element
     */
    T & back();
  };

}

#include <stl_queue.hxx>

#endif// __STL_QUEUE_H__
