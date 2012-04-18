#include <deque>
#include <cstdio>
#include <libtest.h>
#include <cstdlib>
#include <algorithm>

using namespace std;


// test [] operator and default initialization
void test1() {
  deque<int> d (10); // should be init to 0
  bool ok = true;

  for (unsigned int i = 0; i< 10; i++) {
    if (d[i] != 0) {
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
  deque<int> d (10); // should be init to 0
  bool ok = true;

  for (unsigned int i = 0; i< 10; i++) {
    if (d.at(i) != 0) {
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

// test pop_back and pop_front on an empty deque
void test3() {
  deque<int> d;
  d.pop_back();
  d.pop_front();
  
  //ERROR("test pop on an empty dequeOK\n");
}

// test push_front
void test4() {
  deque<int> d;

  d.push_front(1);
  d.push_front(2);
  if (d[0] == 2 && d[1] == 1) {
    
  }
  else {
    ERROR("test push_front\n");
    exit(1);
  }
}

// test push_back
void test5() {
  deque<int> d;

  d.push_back(1);
  d.push_back(2);
  if (d[0] == 1 && d[1] == 2) {
    
  }
  else {
    ERROR("test push_back\n" );
    exit(1);
  }
}

// test size on empty deque and on 10 element deque
void test6() {
  deque<int> d0(10);
  deque<int> d1;

  int s0 = d0.size();
  int s1 = d1.size();
  if (s0 == 10 && s1 == 0) {
    
  }
  else {
    ERROR("test size\n");
    exit(1);
  }
}

// test iterator
void test7() {
  deque<int> d;
  d.push_back(1);
  d.push_back(2);
  deque<int>::iterator it = d.begin();

  if (*it == 1 && *(++it) == 2 && ++it == d.end()) {
    
  }
  else {
    ERROR("test iterator\n" );
    exit(1);
  }
}

// test reverse iterator
void test8() {
  deque<int> d;
  d.push_back(1);
  d.push_back(2);
  deque<int>::reverse_iterator it = d.rbegin();

  if (*it == 2 && *(++it) == 1 && ++it == d.rend()) {
    
  }
  else {
    ERROR("test reverse iterator\n" );
    exit(1);
  }
}

// test resize
void test9() {
  deque<int> d(10);
  d.resize(5);
  int s1 = d.size();
  d.resize(11);
  int s2 = d.size();
  if (s1 == 5 && s2 == 11) {
    
  }
  else {
    ERROR("test resize\n" );
    exit(1);
  }
}

// test empty
void test10 () {
  deque<int> d;
  if (d.empty()) {
    
  }
  else {
    ERROR("test empty\n");
    exit(1);
  }
}

// test front and back
void test11 () {
  deque<int> d;
  d.push_front(1);
  d.push_back(2);
  if (d.front() == 1 && d.back() == 2) {
    
  }
  else {
    ERROR("test front and back\n");
    exit(1);
  }
}

// test popfront
void test12 () {
  deque<int> d;
  d.push_front(1);
  d.push_back(2);
  d.pop_front();
  int s = d.size();
  if (d[0] == 2 && s == 1) {
    
  }
  else {
    ERROR("test pop_front\n");
    exit(1);
  }
}

// test popback
void test13 () {
  deque<int> d;
  d.push_front(1);
  d.push_back(2);
  d.pop_back();
  int s = d.size();
  if (d[0] == 1 && s == 1) {
    
  }
  else {
    ERROR("test pop_back\n");
    exit(1);
  }
}

// test clear
void test14() {
  deque<int> d;
  d.push_front(1);
  d.push_back(2);
  d.clear();
  if (d.size() == 0) {
    
  }
  else {
    ERROR("test clear\n");
    exit(1);
  }
}

// test swap
void test15() {
  deque<int> d1;
  deque<int> d2;
  d1.push_front(1);
  d1.push_back(2);
  d2.push_front(2);
  d2.push_back(1);
  d1.swap(d2);

  if (d1[0] == 2 && d1[1] == 1 && d2[0] == 1 && d2[1] == 2) {
    
  }
  else {
    ERROR("test swap\n");
    exit(1);
  }
}

// test erase
void test16() {
  deque<int> d;
  d.push_back(1);
  d.push_back(2);
  d.push_back(3);
  d.push_back(4);

  d.erase(d.begin() + 4);
  d.erase(d.begin());
  
  if (d.front() == 2 && d.back() == 3) {
    
  }
  else {
    ERROR("test erase\n");
    exit(1);
  }
  
}

// test insert
void test17() {
  deque<int> d;
  d.push_back(2);
  d.push_back(4);

  d.insert(d.begin(),1);
  d.insert(d.begin() + d.size() , 5);
  d.insert(d.begin() + 2, 3);
  
  if (d[0] == 1 && d[1] == 2 && d[2] == 3 && d[3] == 4 && d[4] == 5) {
    
  }
  else {
    ERROR("test insert\n");
    exit(1);
  }
  
}

// test sort
void test18() {
  deque<int> d;
  for (int i = 0; i < NBITER; ++i) {
    d.push_back(rand());
  }

  sort(d.begin(), d.end());

  int prev = d.front();
  for (deque<int>::iterator it = d.begin()+1; it != d.end(); ++it) {
    if (prev > *it) {
      ERROR ("invalid Sort");
      exit(1);
    }
    prev = *it;
  }
  
}


int main () {
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
  test15();
  test16();
  test17();
  test18();
  return 0;
}
