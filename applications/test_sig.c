#include <stdio.h>
#include <stdlib.h>
#include <process.h>

void kill_handler(int signum)
{
	printf("SIGKILL recu.\n");
	return;
}
void usr_handler(int signum)
{
	printf("SIGUSR1 recu.\n");
	return;
}

int main(int argc, char** argv)
{
	int pid = get_pid();
	
	signal(SIGKILL, kill_handler);
	signal(SIGUSR1, usr_handler);
	
	kill(pid, SIGUSR1);
	while(1);
	return 0;
}
