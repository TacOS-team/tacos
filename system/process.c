#include <types.h>
#include <stdlib.h>
#include <stdio.h> 
#include <syscall.h>
#include <process.h>

#define GET_PROCESS 0

void exit(uint32_t value)
{ 
	syscall(SYS_EXIT,value,0,0);
	while(1); // Pour ne pas continuer à executer n'importe quoi alors que le processus est sensé être arrété
}

uint32_t get_pid()
{
	int pid;
	syscall(SYS_GETPID,(uint32_t)&pid, 0, 0);
	return pid;
}

void kill(uint32_t pid)
{
	syscall(SYS_KILL,pid,0,0);
}

void exec(paddr_t prog, char* name)
{
	syscall(SYS_EXEC, (uint32_t)prog, (uint32_t)name, (uint32_t)NULL);
}

process_t* get_process(int pid)
{
	process_t* temp;
	syscall(SYS_PROC,(uint32_t)GET_PROCESS, (uint32_t)pid, (uint32_t) &temp);
	return temp;
}

