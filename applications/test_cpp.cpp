#include <cstdio>
#include <cunistd>

#include <vector>
#include <string>


void testVector() {
  char c = 'a';

  std::vector<char> v (5, 'A');

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

  for (int i = 0; i < 10; ++i)
  {
    v.pop_back();
    printf("taille vector : %d\n", v.size());
  }

  for (size_t i = 0; i < v.size(); ++i)
  {
    printf("%c", v[i]);
  }
  printf("\n");

  for (size_t i = 0; i < v.size(); ++i)
  {
    printf("%c", v.at(i));
  }
  printf("\n");

  printf("front : %c\n", v.front());
  printf("back  : %c\n", v.back());


  v.insert(4, 'Z');
  printf("\ninsert (4, Z)\n");

  for (size_t i = 0; i < v.size(); ++i)
  {
    printf("%c", v[i]);
  }
  printf("\n");

  v.insert(2, 4, 'X');
  printf("insert (2, 4, X)\n");

  for (size_t i = 0; i < v.size(); ++i)
  {
    printf("%c", v[i]);
  }
  printf("\n");

  v.erase(2, 2);
  printf("erase (2, 2) (2 X en moins)\n");

  for (size_t i = 0; i < v.size(); ++i)
  {
    printf("%c", v[i]);
  }
  printf("\n");

  v.insert(2, 100, 'O');
  printf("insert (2, 100, O)\n");

  for (size_t i = 0; i < v.size(); ++i)
  {
    printf("%c", v[i]);
  }
  printf("\n");

  v.insert(1000, 1, 'G');
  printf("insert (1000, G, G)\n");

  for (size_t i = 0; i < v.size(); ++i)
  {
    printf("%c", v[i]);
  }
  printf("\n");
  v.reserve(8);

  printf("reserve(8)\n");

  for (size_t i = 0; i < v.size(); ++i)
  {
    printf("%c", v[i]);
  }

  printf("front : %c\n", v.front());
  printf("back  : %c\n", v.back());


  printf("max size pour char  : %u\n", v.max_size());

  std::vector<int> v2;
  printf("max size pour int  : %u\n", v2.max_size());
  //printf("taille demandée %d, taille réelle  : %d\n", v2.size(), v2.max_size());

}// testVector()

void testString() {
  std::string str ("Test string...");

  printf("Taille : %d\nContenu : %s\n", str.length(), str.c_str());

}// testString()

int main() {
  testString();

	return 0;

}// main()
