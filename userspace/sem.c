#include "include/sem.h"
#include "include/syscall.h"

#define MAX_SEM 256
#define KSEM_GET 1
#define KSEM_CREATE 2
#define KSEM_DEL 3
#define KSEM_P 4
#define KSEM_V 5

int semget(uint8_t key)
{
	int ret;
	int data = key;
	syscall(SYS_SEMCTL, KSEM_GET, (uint32_t)data, (uint32_t)&ret);
	return ret;
}

int semcreate(uint8_t key)
{
	int ret;
	int data = key;
	syscall(SYS_SEMCTL, KSEM_CREATE, (uint32_t)data, (uint32_t)&ret);
	return ret;
}

int semdel(uint32_t semid)
{
	int ret;
	syscall(SYS_SEMCTL, KSEM_DEL, semid, (uint32_t)&ret);
	return ret;
}

int semP(uint32_t semid)
{
	int ret;
	syscall(SYS_SEMCTL, KSEM_P, semid, (uint32_t)&ret);
	return ret;
}

int semV(uint32_t semid)
{
	int ret;
	syscall(SYS_SEMCTL, KSEM_V, semid, (uint32_t)&ret);
	return ret;
}

