#include <sem.h>
#include <syscall.h>

#define MAX_SEM 256
#define KSEM_GET 1
#define KSEM_CREATE 2
#define KSEM_DEL 3
#define KSEM_P 4
#define KSEM_V 5

int semget(uint8_t key)
{
	syscall(SYS_SEMCTL, KSEM_GET, 0, 0);
}

int semcreate(uint8_t key)
{
	syscall(SYS_SEMCTL, KSEM_CREATE, 0, 0);
}

int semdel(uint8_t semid)
{
	syscall(SYS_SEMCTL, KSEM_DEL, 0, 0);
}

int semP(uint8_t semid)
{
	syscall(SYS_SEMCTL, KSEM_P, 0, 0);
}

int semV(uint8_t semid)
{
	syscall(SYS_SEMCTL, KSEM_V, 0, 0);
}

