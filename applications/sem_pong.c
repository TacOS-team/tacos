#include <sem.h>
#include <stdio.h>

int main()
{
	int semid1 = semget(4);
	int semid2 = semget(42);
	int pid = get_pid();
	
	int i;
	while (1) {
		semP(semid1);
		for (i = 0; i < 0x005FFFFE; i++) {
		}
		printf("%d:Pong!\n",pid);
		semV(semid2);

	}
}

