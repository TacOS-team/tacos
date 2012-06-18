
#include <utility>
#include <cstdio>
#include <libtest.h>

using namespace std;


int main() {
  pair< double, int > s;
  s = make_pair (20.0, 20);
  if (s.first != 20.0) {
    ERROR("First invalid")
    exit(1);
  }
  if (s.second != 20) {
    ERROR("Second invalid")
    exit(1);
  }
  s.first = 66.3;
  s.second = 66.3;
  if (s.first != 66.3) {
    ERROR("First invalid")
    exit(1);
  }
  if (s.second != 66) {
    ERROR("Second invalid")
    exit(1);
  }
  pair< double, int > s2 = make_pair (20.0, 20);
  if (s2 > s) {
    ERROR("Supp invalid")
    exit(1);
  }
  if (s < s2) {
    ERROR("Inf invalid")
    exit(1);
  }
  s = s2;
  if (s != s2) {
    ERROR("Different invalid")
    exit(1);
  }
  if (!(s == s2)) {
    ERROR("Equals invalid")
    exit(1);
  }
  if (s > s2) {
    ERROR("Supp invalid")
    exit(1);
  }
  if (!(s >= s2)) {
    ERROR("Supp or equals invalid")
    exit(1);
  }
  return 0;
}
