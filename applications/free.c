#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
	int div = 1;
	if (argc > 1 && argv[1][0] == '-') {
		if (argv[1][1] == 'm' && !argv[1][2]) {
			div = 1024;
		}
	}


	FILE* f = fopen("/proc/meminfo", "r");
	char name[20];
	int value;
	char unit[3];

	int mtotal, mfree;

	while (fscanf(f, "%s %d %s", name, &value, unit) > 1) {
		if (strcmp(name, "MemTotal:") == 0) {
			mtotal = value;
		} else if (strcmp(name, "MemFree:") == 0) {
			mfree = value;
		}

	}
	printf("\ttotal\tused\tfree\n");
	printf("Mem:\t%d\t%d\t%d\n", mtotal / div, (mtotal - mfree) / div, mfree / div);


	fclose(f);

	return 0;
}
