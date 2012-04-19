#ifndef __STL_STACK_HXX__
#define __STL_STACK_HXX__

#include <stl_stack.h>

#define TEMPLATE template < class T, class Container >
#define STACK std::stack<T, Container>

TEMPLATE
STACK::stack(const Container& ctnr/* = Container()*/) : container(ctnr) {}

TEMPLATE
bool STACK::empty() {
  return container.empty();
}

TEMPLATE
size_t STACK::size() {
  return container.size();
}

TEMPLATE
void STACK::pop() {
  if (container.empty()) return; // TODO throw exception
  container.pop_back();
}

TEMPLATE
void STACK::push(const T & elem) {
  // TODO gestion max size
  container.push_back(elem);
}

TEMPLATE
T & STACK::top() {
  return container.back();
}

#undef TEMPLATE

#endif // __STL_STACK_HXX__
