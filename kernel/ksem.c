#include <ksem.h>
#include <ksyscall.h>

#define MAX_SEM 256
#define KSEM_GET 1
#define KSEM_CREATE 2
#define KSEM_DEL 3
#define KSEM_P 4
#define KSEM_V 5
#define SYS_SEMCTL 7

struct waiting_proc_t
{
	struct waiting_proc_t* next;
};

struct sem_t
{
	int value;
	uint8_t allocated;
	struct waiting_proc_t* waiting;
};

static struct sem_t semaphores[MAX_SEM];

int init_semaphores()
{
	int i;
	for(i=0 ; i<MAX_SEM ; i++)
	{
		semaphores[i].value = 0;
		semaphores[i].allocated = 0;
		semaphores[i].waiting = NULL;
	}

	syscall_set_handler(SYS_SEMCTL, ksem_syscallhandle);
}

void ksem_syscallhandle(uint32_t param1, uint32_t param2, uint32_t param3)
{
	switch(param1)
	{
		case KSEM_GET:
			break;
		case KSEM_CREATE:
			break;
		case KSEM_DEL:
			break;
		case KSEM_P:
			break;
		case KSEM_V:
			break;
		default:
			break;
	}	
}

int ksemget(uint8_t key)
{
}

int ksemcreate(uint8_t key)
{
}

int ksemdel(uint8_t key)
{
}

int ksemP(uint8_t key)
{
}

int ksemV(uint8_t key)
{
}


