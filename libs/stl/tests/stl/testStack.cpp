#include <stack>
#include <cstdio>
#include <vector>
#include <libtest.h>

using namespace std;

template < class Container >
void test1(stack< int, Container > & s) {
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
  if (s.top() != 5) {
    ERROR("Top invalide !!!\n");
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
    if (s.top() != i) {
      ERROR("Top invalide !!!\n");
      exit(1);
    }
  }
  for (int i = nbIter-1; i > 0; --i) {
    if (s.size() != (size_t)i) {
      ERROR("Size invalide !!!\n");
      exit(1);
    }
    if (s.top() != i) {
      ERROR("Top invalide !!!\n");
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
  stack< int > s;
  stack< int, vector < int > > sv;

  test1(s);
  test1< vector < int > >(sv);
  return 0;
}
