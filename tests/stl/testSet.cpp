
#include <set>
#include <cstdio>
#include <libtest.h>

using namespace std;

int nbIter = NBITER;

void testErase() {
  set<int> m;
  for(int i = 0; i < nbIter; ++i) {
    m.insert(i);
      if(m.find(i) == m.end() || *(m.find(i)) != i) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  // erase from first to last
  for(int i = 0; i < nbIter; ++i) {
    m.erase(m.find(i));
    for(int j = i+1; j < nbIter; ++j) {
      if(m.find(j) == m.end() || *(m.find(j)) != j) {
        ERROR("opérator [] invalid")
        exit(1);
      }
    }
  }
  if(!m.empty()) {
    ERROR("Empty invalid")
    exit(1);
  }
  for(int i = 0; i < nbIter; ++i) {
    m.insert(i);
    if(m.find(i) == m.end()) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  // erase from last to first
  for(int i = 0; i < nbIter; ++i) {
    m.erase(m.find(i));
    for(int j = nbIter-1; j > i; --j) {
      if(m.find(j) == m.end() || *(m.find(j)) != j) {
        ERROR("opérator [] invalid")
        exit(1);
      }
    }
  }
  if(!m.empty()) {
    ERROR("Empty invalid")
    exit(1);
  }
  for(int i = 0; i < nbIter; ++i) {
    m.insert(i);
    if(m.find(i) == m.end()) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  // erase from first to last
  for(int i = 0; i < nbIter; ++i) {
    m.erase(i);
    for(int j = i+1; j < nbIter; ++j) {
      if(m.find(j) == m.end() || *(m.find(j)) != j) {
        ERROR("opérator [] invalid")
        exit(1);
      }
    }
  }
  if(!m.empty()) {
    ERROR("Empty invalid")
    exit(1);
  }
  for(int i = 0; i < nbIter; ++i) {
    m.insert(i);
    if(m.find(i) == m.end()) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  // erase from last to first
  for(int i = 0; i < nbIter; ++i) {
    m.erase(i);
    for(int j = nbIter-1; j > i; --j) {
      if(m.find(j) == m.end() || *(m.find(j)) != j) {
        ERROR("opérator [] invalid")
        exit(1);
      }
    }
  }
  if(!m.empty()) {
    ERROR("Empty invalid")
    exit(1);
  }
  for(int i = 0; i < nbIter; ++i) {
    m.insert(i);
    if(m.find(i) == m.end()) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  set<int>::iterator deb, fin;
  deb = m.begin();
  ++deb;
  fin = m.end();
  --fin;
  m.erase(deb, fin);
  if(m.size() != 2) {
    ERROR("Size invalid")
    exit(1);
  }
  if(m.find(0) == m.end() || *(m.find(0)) != 0) {
    ERROR("m.find(0) invalide")
    exit(1);
  }
  if(m.find(nbIter-1) == m.end() || *(m.find(nbIter-1)) != nbIter-1) {
    ERROR("m.find(9) invalide")
    exit(1);
  }
  m.erase(m.begin(), m.end());
  if(m.size() != 0) {
    ERROR("Size invalid")
    exit(1);
  }
  if(!m.empty()) {
    ERROR("Empty invalid")
    exit(1);
  }
}

int main() {
  set<int> m;
  if (!m.empty()) {
    ERROR("m non vide au départ")
    exit(1);
  }
  if (m.count(10) != 0) {
    ERROR("Count invalide")
    exit(1);
  }
  m.insert(10);
  if (m.find(10) == m.end()) {
    ERROR("m.find(10) invalid")
    exit(1);
  }
  if (m.empty()) {
    ERROR("m vide après une insertion")
    exit(1);
  }
  if (m.count(10) != 1) {
    ERROR("Count invalide")
    exit(1);
  }
  if (m.count(50) != 0) {
    ERROR("Count invalide")
    exit(1);
  }
  m.clear();
  if (!m.empty()) {
    ERROR("m non vide après un clear")
    exit(1);
  }
  if (m.count(10) != 0) {
    ERROR("Count invalide après clear")
    exit(1);
  }
  if (m.count(50) != 0) {
    ERROR("Count invalide clear")
    exit(1);
  }
  testErase();
  return 0;
}
