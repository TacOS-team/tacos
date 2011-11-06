#include <cstdio>

#include <vector>



int main() {
  std::vector<char> v;

  char c = 'a';

  printf("taille vector : %d\n", v.size());

  for (int i = 0; i < 26; ++i)
  {
    v.push_back(c++);
    printf("taille vector : %d\n", v.size());
  }


  for (size_t i = 0; i < v.size(); ++i)
  {
    printf("%c", v[i]);
  }
  printf("\n");

	return 0;
}
