#include "include/test_stdio.h"
#include "include/test_sig.h"

int main() {
	test_io();
	test_sprintf();
	test_malloc();
	test_sig();
	return 0;
}
