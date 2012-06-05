
#include <map>
#include <string>
#include <cstdio>
#include <libtest.h>

using namespace std;

int nbIter = 10;

void testErase() {
  map<int, float> m;
  for(int i = 0; i < nbIter; ++i) {
    m[i] = i;
    if(m[i] != i) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  // erase from first to last
  for(int i = 0; i < nbIter; ++i) {
    m.erase(m.find(i));
    for(int j = i+1; j < nbIter; ++j) {
      if(m[j] != j) {
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
    m[i] = i;
    if(m[i] != i) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  // erase from last to first
  for(int i = 0; i < nbIter; ++i) {
    m.erase(m.find(i));
    for(int j = nbIter-1; j > i; --j) {
      if(m[j] != j) {
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
    m[i] = i;
    if(m[i] != i) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  // erase from first to last
  for(int i = 0; i < nbIter; ++i) {
    m.erase(i);
    for(int j = i+1; j < nbIter; ++j) {
      if(m[j] != j) {
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
    m[i] = i;
    if(m[i] != i) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  // erase from last to first
  for(int i = 0; i < nbIter; ++i) {
    m.erase(i);
    for(int j = nbIter-1; j > i; --j) {
      if(m[j] != j) {
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
    m[i] = i;
    if(m[i] != i) {
      printf("%d\n", i);
      ERROR("opérator [] invalid")
      exit(1);
    }
  }
  map<int, float>::iterator deb, fin;
  deb = m.begin();
  ++deb;
  fin = m.end();
  --fin;
  m.erase(deb, fin);
  if(m.size() != 2) {
    ERROR("Size invalid")
    exit(1);
  }
  if (m[0] != 0.0) {
    ERROR("Count invalide")
    exit(1);
  }
  if (m[9] != 9.0) {
    ERROR("Count invalide")
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
  map<int, float> m;
  if (!m.empty()) {
    ERROR("m non vide au départ")
    exit(1);
  }
  if (m.count(10) != 0) {
    ERROR("Count invalide")
    exit(1);
  }
  m[10] = 10.5;
  if (m[10] != 10.5) {
    ERROR("m[10] invalid")
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
