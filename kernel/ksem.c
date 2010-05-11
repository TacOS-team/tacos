#include <ksem.h>
#include <ksyscall.h>
#include <stdio.h>
#include <kmalloc.h>

#define MAX_SEM 256
#define KSEM_GET 1
#define KSEM_CREATE 2
#define KSEM_DEL 3
#define KSEM_P 4
#define KSEM_V 5
#define SYS_SEMCTL 8

struct semid_t
{
	uint32_t pid;
	uint32_t id;
	struct semid_t* next;
};

struct sem_t
{
	int value;
	uint8_t allocated;
	struct semid_t* waiting;
	struct semid_t* idle;
};

static struct sem_t semaphores[MAX_SEM];
static int  next_semid = 1;

// Note sur les semid
// 8 most significant bits = key
// 24 least .............. = valeur incrementee a chaque fois

int init_semaphores()
{
	int i;
	for(i=0 ; i<MAX_SEM ; i++)
	{
		semaphores[i].value = 0;
		semaphores[i].allocated = 0;
		semaphores[i].waiting = NULL;
		semaphores[i].idle = NULL;
	}

	syscall_set_handler(SYS_SEMCTL, ksem_syscallhandle);
	return 0;
}

static int ksemget(uint8_t key, uint32_t pid)
{
	int ret = -1;
	struct semid_t* sem;
	struct semid_t* ptr;

	kprintf("semget key %d pid %d \n",key,pid);
	if(semaphores[key].allocated)
	{
		ret = next_semid | key<<24;
		next_semid++;

		sem = (struct semid_t*) kmalloc(sizeof(struct semid_t));
		sem->pid = pid;
		sem->id = ret;
		sem->next = NULL;

		// On enfile le nouveau processus 
		ptr = semaphores[key].idle;
		while(ptr->next != NULL)
			ptr = ptr->next;
		ptr->next = sem;
	}

	kprintf("returning semid %d\n",ret);
	return ret;
}

static int ksemcreate(uint8_t key, uint32_t pid)
{
	int ret = -1;
	struct semid_t* sem;

	kprintf("semcreate key %d pid %d \n",key,pid);
	if(!semaphores[key].allocated)
	{
		ret = next_semid | key<<24;
		next_semid++;
		semaphores[key].allocated = 1;
		semaphores[key].value = 1;

		sem = (struct semid_t*) kmalloc(sizeof(struct semid_t));
		sem->pid = pid;
		sem->id = ret;
		sem->next = NULL;

		semaphores[key].idle = sem;
	}

	kprintf("returning semid %d\n",ret);
	return ret;
}

static int ksemdel(uint32_t semid)
{
	struct semid_t* to_delete;
	struct semid_t* next;
	int key = (semid & 0xFF000000) >> 24;
	kprintf("semdel semid %d \n",semid);
	if(semaphores[key].allocated)
	{
		semaphores[key].allocated = 0;

		// /!\ il faudrait relancer les processus en attente
		to_delete = semaphores[key].waiting;
		while(to_delete != NULL)
		{
			next = to_delete->next;
			kfree(to_delete);
			to_delete = next;
		}
		semaphores[key].waiting = NULL;

		to_delete = semaphores[key].idle;
		while(to_delete != NULL)
		{
			next = to_delete->next;
			kfree(to_delete);
			to_delete = next;
		}
		semaphores[key].idle = NULL;
		return 0;
	}

	return -1;
}

static int ksemP(uint32_t semid)
{
	int pid;
	int key = (semid & 0xFF000000) >> 24;
	struct semid_t* ptr;

	if(semaphores[key].allocated)
	{	
		// On cherche le pid correspondant 
		ptr = semaphores[key].idle;
		while(ptr != NULL)
		{
			if(ptr->id == semid)
				break;
			else
				ptr = ptr->next;
		}
		if(ptr != NULL)
		{
			pid = ptr->pid;
		}
		else
		{
			kprintf("semP semid %d for key %d no corresponding pid %d\n",semid,key);
			return -1;
		}
		kprintf("semP semid %d for key %d from pid %d\n",semid, key,pid);
		// Normalement on stop le process gentiment avec le scheduler
		while(semaphores[key].value < 1);
		semaphores[key].value--;

		return 0;
	}
	return -1;
}

static int ksemV(uint32_t semid)
{
	int pid;
	int key = (semid & 0xFF000000) >> 24;
	struct semid_t* ptr;

	if(semaphores[key].allocated)
	{
		// On cherche le pid correspondant 
		ptr = semaphores[key].idle;
		while(ptr != NULL)
		{
			if(ptr->id == semid)
				break;
			else
				ptr = ptr->next;
		}
		if(ptr != NULL)
		{
			pid = ptr->pid;
		}
		else
		{
			kprintf("semV semid %d for key %d no corresponding pid %d\n",semid,key);
			return -1;
		}
		kprintf("semV semid %d for key %d from pid %d\n",semid, key,pid);
		// On incremente le semaphore
		semaphores[key].value++;
		return 0;
	}
	return -1;
}


void ksem_syscallhandle(uint32_t param1, uint32_t param2, uint32_t param3)
{
	switch(param1)
	{
		case KSEM_GET:
			*((int*)param3) = ksemget(((uint32_t*)param2)[0],((uint32_t*)param2)[1]);
			break;
		case KSEM_CREATE:
			*((int*)param3) = ksemcreate(((uint32_t*)param2)[0],((uint32_t*)param2)[1]);
			break;
		case KSEM_DEL:
			*((int*)param3) = ksemdel(param2);
			break;
		case KSEM_P:
			*((int*)param3) = ksemP(param2);
			break;
		case KSEM_V:
			*((int*)param3) = ksemV(param2);
			break;
		default:
			break;
	}	
}

