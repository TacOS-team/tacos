#include <stack>
#include <cstdio>
#include <vector>
#include <stdlib.h>
// g++ -Wall testStack.cpp -I. -I../../libc/include/ -I../../libc/include/sys/

using namespace std;

void erreur(const char * mess) {
  printf("ERREUR ligne %d : %s\n", __LINE__, mess);
}

template < class Container >
void test1(stack< int, Container > & s) {
  if (!s.empty()) {
    erreur("NON Empty au départ !!!\n");
    exit(1);
  }
  if (s.size() != 0) {
    erreur("Size invalide !!!\n");
    exit(1);
  }
  s.push(5);
  if (s.empty()) {
    erreur("Empty invalide !!!\n");
    exit(1);
  }
  if (s.top() != 5) {
    erreur("Top invalide !!!\n");
    exit(1);
  }
  if (s.size() != 1) {
    erreur("Size invalide !!!\n");
    exit(1);
  }
  s.pop();
  if (!s.empty()) {
    erreur("Empty invalide !!!\n");
    exit(1);
  }
  if (s.size() != 0) {
    erreur("Size invalide !!!\n");
    exit(1);
  }
  int nbIter = 20000;
  for (int i = 1; i < nbIter; ++i) {
    s.push(i);
    if (s.size() != (size_t)i) {
      erreur("Size invalide !!!\n");
      exit(1);
    }
    if (s.top() != i) {
      erreur("Top invalide !!!\n");
      exit(1);
    }
  }
  for (int i = nbIter-1; i > 0; --i) {
    if (s.size() != (size_t)i) {
      erreur("Size invalide !!!\n");
      exit(1);
    }
    if (s.top() != i) {
      erreur("Top invalide !!!\n");
      exit(1);
    }
    s.pop();
  }
  if (!s.empty()) {
    erreur("Empty invalide !!!\n");
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
