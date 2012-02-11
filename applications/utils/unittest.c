#include <stdio.h>
#include <string.h>

static void padding(int c) {
	while (c-- > 0) {
		printf(" ");
	}
}

void unit_test_int(const char* msg, int attendu, int obtenu) {
	int len = strlen(msg);
	printf("%s", msg);
	if (attendu == obtenu) {
		padding(80 - len - 4);
		printf("[OK]\n");
	} else {
		padding(80 - len - 7);
		printf("[ERROR]\n");
		printf("Attendu : %d, Obtenu : %d\n", attendu, obtenu);
	}
}

void unit_test_str(const char* msg, const char* attendu, const char* obtenu) {
	int len = strlen(msg);
	printf("%s", msg);
	if (strcmp(attendu, obtenu) == 0) {
		padding(80 - len - 4);
		printf("[OK]\n");
	} else {
		padding(80 - len - 7);
		printf("[ERROR]\n");
		printf("Attendu : %s, Obtenu : %s\n", attendu, obtenu);
	}
}


