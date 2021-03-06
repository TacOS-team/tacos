#include <queue>
#include <cstdio>
#include <vector>
#include <libtest.h>

using namespace std;

template < class Container >
void test1(queue< int, Container > & s) {
  if (!s.empty()) {
    ERROR("NON Empty au départ !!!\n");
    exit(1);
  }
  if (s.size() != 0) {
    ERROR("Size invalide !!!\n");
    exit(1);
  }
  s.push(5);
  if (s.empty()) {
    ERROR("Empty invalide !!!\n");
    exit(1);
  }
  if (s.front() != 5) {
    ERROR("Front invalide !!!\n");
    exit(1);
  }
  if (s.back() != 5) {
    ERROR("Back invalide !!!\n");
    exit(1);
  }
  if (s.size() != 1) {
    ERROR("Size invalide !!!\n");
    exit(1);
  }
  s.pop();
  if (!s.empty()) {
    ERROR("Empty invalide !!!\n");
    exit(1);
  }
  if (s.size() != 0) {
    ERROR("Size invalide !!!\n");
    exit(1);
  }
  int nbIter = 20000;
  for (int i = 1; i < nbIter; ++i) {
    s.push(i);
    if (s.size() != (size_t)i) {
      ERROR("Size invalide !!!\n");
      exit(1);
    }
    if (s.back() != i) {
      ERROR("Back invalide !!!\n");
      exit(1);
    }
    if (s.front() != 1) {
      ERROR("Front invalide !!!\n");
      exit(1);
    }
  }
  for (int i = nbIter-1; i > 0; --i) {
    if (s.size() != (size_t)i) {
      ERROR("Size invalide !!!\n");
      exit(1);
    }
    if (s.front() != nbIter-i) {
      ERROR("Front invalide !!!\n");
      exit(1);
    }
    if (s.back() != nbIter-1) {
      ERROR("Back invalide !!!\n");
      exit(1);
    }
    s.pop();
  }
  if (!s.empty()) {
    ERROR("Empty invalide !!!\n");
    exit(1);
  }
}


int main() {
  queue< int > s;
  //queue< int, vector < int > > sv;

  test1(s);
  //test1< vector < int > >(sv);
  return 0;
}
