#ifndef __STL_STACK_H__
#define __STL_STACK_H__

#include <deque>

namespace std {

  template < class T, class Container = deque<T> >
  class stack {
   protected:
    Container container;

   public:
    stack (const Container& ctnr = Container()) : container(ctnr) { }

    bool empty() {
      return container.empty();
    }

    size_t size() {
      return container.size();
    }

    void pop() {
      if (container.empty()) return; // TODO throw exception
      container.pop_back();
    }

    void push(const T & elem) {
      // TODO gestion max size
      container.push_back(elem);
    }

    T & top() {
      return container.back();
    }
  };
}

#endif// __STL_STACK_H__
