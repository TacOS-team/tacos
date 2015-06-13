#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
	if (argc < 3) {
		printf("Usage: %s SIGNAL PID\n", argv[0]);
		return 1;
	}
	pid_t pid = atoi(argv[2]);
	return kill(pid, atoi(argv[1]));
}
