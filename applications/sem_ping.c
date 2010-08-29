#include <sem.h>
#include <stdio.h>

int main()
{
	int semid1 = semcreate(4);
	int semid2;
	int pid = get_pid();

	semid2 = semcreate(42);
	semP(semid1);
	
	unsigned int i = 0;

	while (1) {
		semP(semid2);
		for (i = 0; i < 0x005FFFFE; i++) {
		}
		printf("%d:Ping?\n",pid);
		semV(semid1);
	}
	return 0;
}
