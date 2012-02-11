#include <stdio.h>
#include <time.h>

int main() {
	time_t date = time(NULL);
	printf("%s", ctime(&date));
	return 0;
}
