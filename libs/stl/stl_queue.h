#ifndef __STL_QUEUE_H__
#define __STL_QUEUE_H__

#include <queue>
#include <deque>

namespace std {

  template < class T, class Container = deque<T> >
  class queue {
   protected:
    Container container;

   public:
    queue (const Container& ctnr = Container()) : container(ctnr) { }

    bool empty() {
      return container.empty();
    }

    size_t size() {
      return container.size();
    }

    void pop() {
      if (container.empty()) return; // TODO throw exception
      container.pop_front();
    }

    void push(const T & elem) {
      // TODO gestion max size
      container.push_back(elem);
    }

    T & front() {
      return container.front();
    }

    T & back() {
      return container.back();
    }
  };
}

#endif// __STL_QUEUE_H__
