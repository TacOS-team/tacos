/* TODO: Séparer le code de la fifo dans un autre fichier, voir le faire générique pour que ce soit réutilisable pour d'autres types d'IPC */

#include <ksem.h>
#include <kprocess.h>
#include <stdio.h>
#include <kmalloc.h>

#define FIFO_MAX_SIZE 32
#define MAX_SEM 256
#define KSEM_GET 1
#define KSEM_CREATE 2
#define KSEM_DEL 3
#define KSEM_P 4
#define KSEM_V 5
#define SYS_SEMCTL 8

/***********************************
 * 
 * STRUCT DECLARATIONS
 * 
 ***********************************/

typedef struct _sem_fifo_cell{
	int pid;
	struct _sem_fifo_cell* prev;
	struct _sem_fifo_cell* next;
} sem_fifo_cell;

typedef struct {
	int size;
	sem_fifo_cell* head;
	sem_fifo_cell* tail;
}sem_fifo;

typedef struct
{
	int value;
	uint8_t allocated;
	sem_fifo fifo;
}sem_t;

/************************************
 * 
 * GLOBAL DECLARATIONS
 * 
 ************************************/

static sem_t semaphores[MAX_SEM];
static int  next_semid = 1;

/*************************************
 * 
 * CODE
 * 
 *************************************/

/* Initialisation de la fifo */
static void sem_fifo_init(sem_fifo* fifo)
{
	fifo->size = 0;
	fifo->head = NULL;
	fifo->tail = NULL;
}

/* Met un élément dans la file */
static int sem_fifo_put(sem_fifo* fifo, int pid)
{
	int ret = -1;
	sem_fifo_cell* new_cell;
	if(fifo->size < FIFO_MAX_SIZE)
	{
		new_cell = (sem_fifo_cell*) kmalloc(sizeof(sem_fifo_cell));
		new_cell->pid = pid;
		new_cell->prev = NULL;
		
		if(fifo->size == 0)
		{
			new_cell->next = NULL;
			fifo->tail = new_cell;
		}
		else
		{
			new_cell->next = fifo->head;
			fifo->head->prev = new_cell;
		}

		fifo->head = new_cell;
		
		fifo->size ++;
		ret = 0;
	}
	return ret;
}
 
/* Prend le prochain élément dans la file */
static int sem_fifo_get(sem_fifo* fifo)
{
	int pid = -1;
	sem_fifo_cell* temp = fifo->tail;
	if(fifo->size > 0)
	{
		fifo->tail = temp->prev;
		pid = temp->pid;
		kfree(temp);
		fifo->size--;
	}
	return pid;
}

/* Retourne la taille actuelle de la fifo */
static int sem_fifo_size(sem_fifo* fifo)
{
	return fifo->size;
}

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
		sem_fifo_init(&(semaphores[i].fifo));
	}
	return 0;
}

int ksemget(uint8_t key)
{
	int ret = -1;

	/*kprintf("semget key %d pid %d \n",key);*/
	if(semaphores[key].allocated)
	{
		ret = next_semid | key<<24;
		next_semid++;
	}

	/* kprintf("returning semid %d\n",ret);*/
	return ret;
}

int ksemcreate(uint8_t key)
{
	int ret = -1;

	//kprintf("semcreate key %d pid %d \n",key);
	if(key < MAX_SEM && !semaphores[key].allocated)
	{
		ret = next_semid | key<<24;
		next_semid++;
		semaphores[key].allocated = 1;
		semaphores[key].value = 1;
	}

	//kprintf("returning semid %d\n",ret);
	return ret;
}

int ksemcreate_without_key(uint8_t *key)
{
    int ret = -1;
    *key = 0;

    while (*key < MAX_SEM && semaphores[*key].allocated) {
        (*key)++;
    }

    return ksemcreate(*key);
}

int ksemdel(uint32_t semid)
{
	int key = (semid & 0xFF000000) >> 24;
	//kprintf("semdel semid %d \n",semid);
	if(semaphores[key].allocated)
	{
		semaphores[key].allocated = 0;
		
		/* TODO : Liberer tous les processus en attente */
		return 0;
	}

	return -1;
}

int ksemP(uint32_t semid)
{
	int ret = -1;
	int key = (semid & 0xFF000000) >> 24;
	sem_t* sem = &semaphores[key];
	
	if(sem->allocated)
	{	
		/* Si le sémaphore est libre, on le prend, sinon, on attend */
		if(sem->value >= 1) {
			sem->value--;
		}
		else {
			process_t* proc = get_current_process();
			/* Si on attend, on met le pid dans la file d'attente pour pouvoir traiter le processus quand le semaphore sera libre */
			sem_fifo_put(&(sem->fifo), proc->pid);
			proc->state = PROCSTATE_WAITING;
			while(proc->state == PROCSTATE_WAITING) asm("hlt");
		}

		ret = 0;
	}
	
	return ret;
}

int ksemV(uint32_t semid)
{
	int ret = -1;
	int key = (semid & 0xFF000000) >> 24;
	process_t* proc;
	sem_t* sem = &semaphores[key];
	
	if(sem->allocated)
	{
		/* Si la file est vide, on incrémente la valeur, sinon, on débloque le premier processus en attente */
		if(sem_fifo_size(&(sem->fifo)) == 0) {
			sem->value++;
		}
		else {
			proc = find_process(sem_fifo_get(&(sem->fifo)));
			proc->state = PROCSTATE_RUNNING;
		}
		ret = 0;
	}
	return ret;
}


void sys_ksem(uint32_t param1, uint32_t param2, uint32_t param3)
{
	switch(param1)
	{
		case KSEM_GET:
			*((int*)param3) = ksemget(param2);
			break;
		case KSEM_CREATE:
			*((int*)param3) = ksemcreate(param2);
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

