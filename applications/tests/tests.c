#include "include/test_stdio.h"
#include "include/test_sig.h"
#include "include/test_malloc.h"

int main() {
	test_io();
	test_sprintf();
	test_sscanf();
	test_malloc();
	test_sig();
	return 0;
}
