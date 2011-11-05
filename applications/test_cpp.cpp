#include <cstdio>

class Test {
public:
	void hello() {
		printf("Hello from C++!\n");
		return;
	}
};

int main() {
	Test t;
	t.hello();

	return 0;
}
