#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/include/unittest.h"

void test_malloc() {
	// Test 1:
	void *a = malloc(100);
	unit_test_int("malloc(100) doit retourner un pointeur non null.", 1, a != NULL);
	free(a);

	// Test 2:
	void *b = malloc(sizeof(char) * 101);
	strcpy(b, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");
	unit_test_str("strcpy dans un buffer alloué avec malloc", "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890", b);
	free(b);

	// Test 3:
	void *c = malloc(sizeof(char) * 1000000);
	strcpy(c, "Test");
	unit_test_str("strcpy dans un buffer de 1mo", "Test", c);
	free(c);
}
