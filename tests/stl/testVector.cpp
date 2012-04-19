#include <vector>
#include <algorithm>
#include <libtest.h>
#include <cstdlib>

using namespace std;

void printVector(std::vector<int> & v) {
  for (size_t i = 0; i < v.size(); ++i) {
    printf("v[%lu] = %d\n", i, v[i]);
  }
}

// test [] operator and default initialization
void test1() {
  vector<int> v(10); // should be init to 0
  bool ok = true;

  for (unsigned int i = 0; i< 10; i++) {
    if (v[i] != 0) {
      ok = false;
      break;
    }
  }
  if (ok) {
    
  } else {
    ERROR("test init and [] operator\n" );
    exit(1);
  }
}

// test at
void test2() {
  vector<int> v(10); // should be init to 0
  bool ok = true;

  for (unsigned int i = 0; i< 10; i++) {
    if (v.at(i) != 0) {
      ok = false;
      break;
    }
  }
  if (ok) {
    
  } else {
    ERROR("test at\n");
    exit(1);
  }
}

// test push_back
void test3() {
  vector<int> v;

  v.push_back(1);
  v.push_back(2);
  if (v[0] == 1 && v[1] == 2) {
    
  }
  else {
    ERROR("test push_back\n" );
    exit(1);
  }
}

// test size on empty vector and on 10 element vector
void test4() {
  vector<int> v0(10);
  vector<int> v1;

  int s0 = v0.size();
  int s1 = v1.size();
  if (s0 == 10 && s1 == 0) {
    
  }
  else {
    ERROR("test size\n");
    exit(1);
  }
}

// test iterator
void test5() {
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  vector<int>::iterator it = v.begin();

  if (*it == 1 && *(++it) == 2 && ++it == v.end()) {
    
  }
  else {
    ERROR("test iterator\n" );
    exit(1);
  }
}

// test reverse iterator
void test6() {
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  vector<int>::reverse_iterator it = v.rbegin();

  if (*it == 2 && *(++it) == 1 && ++it == v.rend()) {
    
  }
  else {
    ERROR("test reverse iterator\n" );
    exit(1);
  }
}

// test empty
void test7() {
  vector<int> v;
  if (v.empty()) {
    
  }
  else {
    ERROR("test empty\n");
    exit(1);
  }
}

// test front and back
void test8() {
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  if (v.front() == 1 && v.back() == 2) {
    
  }
  else {
    ERROR("test front and back\n");
    exit(1);
  }
}

// test pop_back
void test9() {
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.pop_back();
  int s = v.size();
  if (v[0] == 1 && s == 1) {
    
  }
  else {
    ERROR("test pop_back\n");
    exit(1);
  }
}

// test clear
void test10() {
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.clear();
  if (v.size() == 0) {
    
  }
  else {
    ERROR("test clear\n");
    exit(1);
  }
}

// test swap
void test11() {
  vector<int> v1;
  vector<int> v2;
  v1.push_back(1);
  v1.push_back(2);
  v2.push_back(2);
  v2.push_back(1);
  v1.swap(v2);

  if (v1[0] == 2 && v1[1] == 1 && v2[0] == 1 && v2[1] == 2) {
    
  }
  else {
    ERROR("test swap\n");
    exit(1);
  }
}

// test erase
void test12() {
  vector<int> v;
  v.push_back(1);
  v.push_back(2);
  v.push_back(3);
  v.push_back(4);

  v.erase(v.begin() + 3);
  v.erase(v.begin());
  
  if (v.front() == 2 && v.back() == 3) {
    
  }
  else {
    ERROR("test erase\n");
    exit(1);
  }
}

// test insert
void test13() {
  vector<int> v1;
  v1.push_back(1);
  v1.push_back(2);
  v1.push_back(3);
  v1.push_back(4);
  v1.push_back(5);

  vector<int> v2;
  v2.push_back(11);
  v2.push_back(12);
  v2.push_back(13);
  v2.push_back(14);
  v2.push_back(15);

  v1.insert(v1.begin() + v1.size(), v2.begin(), v2.begin() + 1);
  v1.insert(v1.begin() + 3, v2.begin() + 2, v2.begin() + 5);
  v1.insert(v1.begin() + v1.size(), v2.begin(), v2.begin());
  v1.insert(v1.begin(), v2.begin() + 4, v2.end());
  
  if (v1[0] == 15 && v1[1] == 1 && v1[2] == 2 && v1[3] == 3 && v1[4] == 13 && v1[5] == 14 && v1[6] == 15 && v1[7] == 4 && v1[8] == 5 && v1[9] == 11) {
    
  }
  else {
    ERROR("test insert\n");
    exit(1);
  }
}

// test sort
void test14() {
  vector<int> v;

  for (int i = 0; i < NBITER; ++i) {
    v.push_back(rand() % NBITER);
  }

  sort(v.begin(), v.end());

  int prev = v[0];
  for (int i = 1; i < NBITER; ++i) {
    if (prev > v[i]) {
      ERROR ("invalid sort");
      exit(1);
    }
    prev = v[i];
  }

  sort(v.rbegin(), v.rend());

  prev = v[0];
  for (int i = 1; i < NBITER; ++i) {
    if (prev < v[i]) {
      ERROR("invalid sort");
      exit(1);
    }
    prev = v[i];
  }
}

int main() {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  test9();
  test10();
  test11();
  test12();
  test13();
  test14();

  return 0;
}
