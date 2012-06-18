#ifndef __STL_QUEUE_HXX__
#define __STL_QUEUE_HXX__

#include <stl_queue.h>

#define TEMPLATE template < class T, class Container >
#define QUEUE std::queue<T, Container>

TEMPLATE
QUEUE::queue(const Container& ctnr/* = Container()*/) : container(ctnr) {}

TEMPLATE
bool QUEUE::empty() {
  return container.empty();
}

TEMPLATE
size_t QUEUE::size() {
  return container.size();
}

TEMPLATE
void QUEUE::pop() {
  if (container.empty()) return; // TODO throw exception
  container.pop_front();
}

TEMPLATE
void QUEUE::push(const T & elem) {
  // TODO gestion max size
  container.push_back(elem);
}

TEMPLATE
T & QUEUE::front() {
  return container.front();
}

TEMPLATE
T & QUEUE::back() {
  return container.back();
}

#undef TEMPLATE

#endif // __STL_QUEUE_HXX__
