#include <kmalloc.h>
#include <kprocess.h>
#include <scheduler.h>

/**
 * @file round_robin.c
 * 
 */

/*
 * PROTOTYPES
 */
static int rr_initialize(int max);
static process_t* rr_get_next_process();
static process_t* rr_get_current_process();
static int rr_add_process(process_t* proc);
static int rr_delete_process(int pid);
static void rr_inject_idle(process_t* proc);

/*
 * Structure à exporter pour utiliser le code en tant que module scheduler
 */
scheduler_descriptor_t round_robin = {
	.name = "Round robin",
	.initialize = rr_initialize,
	.get_next_process = rr_get_next_process,
	.get_current_process = rr_get_current_process,
	.add_process = rr_add_process,
	.delete_process = rr_delete_process,
	.inject_idle = rr_inject_idle
};

typedef struct _list_cell
{
	 process_t* proc; 
	 struct _list_cell* prev;
	 struct _list_cell* next;
	 
}list_cell;

static list_cell* process_list = NULL;
static list_cell* current_cell = NULL;
static max_process = 0;

static process_t* idle_process = NULL;

static int rr_initialize(int max)
{
	max_process = max;
	return 0;
}

static process_t* rr_get_next_process()
{
	list_cell* aux = current_cell;
	process_t* ret = NULL;
	do
	{
		aux = aux->next;
		if(aux == NULL)
			aux = process_list;
		
		if(aux->proc->state != PROCSTATE_WAITING && aux->proc->state != PROCSTATE_WAITING)
			ret = aux->proc;
		
	}while(aux != current_cell && ret == NULL);
	current_cell = aux;
	return ret;
}

static process_t* rr_get_current_process()
{
	process_t* ret = NULL;
	
	if(idle_process != NULL)
	{
		ret = idle_process;
		idle_process = NULL;
	}
	else if(current_cell != NULL)
		ret = current_cell->proc;
		
	return ret;
}

static int rr_add_process(process_t* proc)
{
	list_cell* aux;
	if(process_list == NULL)
	{
		process_list = kmalloc(sizeof(list_cell));
		process_list->next = NULL;
		process_list->prev = NULL;
		process_list->proc = proc;
		current_cell = process_list;
	}
	else
	{
		aux = kmalloc(sizeof(list_cell));
		aux->prev = NULL;
		aux->next = process_list;
		aux->proc = proc;
		process_list->prev = aux;
		process_list = aux;
	}
	return 0;
}

static int rr_delete_process(int pid)
{
	list_cell* aux = process_list;
	int ret = -1;
	
	while(aux != NULL && aux->proc->pid != pid)
		aux = aux->next;
	
	if(aux != NULL)
	{
		ret = 1;
		
		/* si aux est la tête de la liste */
		if(aux->prev == NULL)
		{
			process_list = aux->next;
			aux->next->prev = NULL;
		}
		else if(aux->next == NULL) /* si aux est la queue de la liste */
			aux->prev->next = NULL;
		else /* dans tous les autres cas */
		{
			aux->prev->next = aux->next;
			aux->next->prev = aux->prev;
		}
		kfree(aux);
	}
	return ret;
}

static void rr_inject_idle(process_t* proc)
{
	idle_process = proc;
}