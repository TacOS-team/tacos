#include <queue>
#include <cstdio>
#include <vector>
#include <stdlib.h>
// g++ -Wall testStack.cpp -I. -I../../libc/include/ -I../../libc/include/sys/

using namespace std;

void erreur(int ligne, const char * mess) {
  printf("ERREUR ligne %d : %s\n", ligne, mess);
}

template < class Container >
void test1(queue< int, Container > & s) {
  if (!s.empty()) {
    erreur(__LINE__, "NON Empty au départ !!!\n");
    exit(1);
  }
  if (s.size() != 0) {
    erreur(__LINE__, "Size invalide !!!\n");
    exit(1);
  }
  s.push(5);
  if (s.empty()) {
    erreur(__LINE__, "Empty invalide !!!\n");
    exit(1);
  }
  if (s.front() != 5) {
    erreur(__LINE__, "Front invalide !!!\n");
    exit(1);
  }
  if (s.back() != 5) {
    erreur(__LINE__, "Back invalide !!!\n");
    exit(1);
  }
  if (s.size() != 1) {
    erreur(__LINE__, "Size invalide !!!\n");
    exit(1);
  }
  s.pop();
  if (!s.empty()) {
    erreur(__LINE__, "Empty invalide !!!\n");
    exit(1);
  }
  if (s.size() != 0) {
    erreur(__LINE__, "Size invalide !!!\n");
    exit(1);
  }
  int nbIter = 20000;
  for (int i = 1; i < nbIter; ++i) {
    s.push(i);
    if (s.size() != (size_t)i) {
      erreur(__LINE__, "Size invalide !!!\n");
      exit(1);
    }
    if (s.back() != i) {
      erreur(__LINE__, "Back invalide !!!\n");
      exit(1);
    }
    if (s.front() != 1) {
      erreur(__LINE__, "Front invalide !!!\n");
      exit(1);
    }
  }
  for (int i = nbIter-1; i > 0; --i) {
    if (s.size() != (size_t)i) {
      erreur(__LINE__, "Size invalide !!!\n");
      exit(1);
    }
    if (s.front() != nbIter-i) {
      erreur(__LINE__, "Front invalide !!!\n");
      exit(1);
    }
    if (s.back() != nbIter-1) {
      erreur(__LINE__, "Back invalide !!!\n");
      exit(1);
    }
    s.pop();
  }
  if (!s.empty()) {
    erreur(__LINE__, "Empty invalide !!!\n");
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
