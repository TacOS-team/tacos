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

void testSort() {

  vector < int > v;

  for (int i = 0; i < NBITER; ++i) {
    v.push_back(rand());
  }

  sort(v.begin(), v.end());

  int prev = v[0];
  for (int i = 1; i < NBITER; ++i) {
    if (prev >= v[i]) {
      ERROR ("invalid sort");
      exit(1);
    }
    prev = v[i];
  }

  //printVector(v);

  sort(v.rbegin(), v.rend());

  //printVector(v);

  prev = v[0];
  for (int i = 1; i < NBITER; ++i) {
    if (prev < v[i]) {
      ERROR ("invalid sort");
      exit(1);
    }
    prev = v[i];
  }
}

int main() {
  testSort();

  return 0;
}
