#include <types.h>
#include <stdio.h>
#include <string.h>
#include "shell_utils.h"

typedef struct
{
	func_ptr func;
	char* name;
}builtin_cmd;

typedef struct _cmd_list_cell
{
	builtin_cmd cmd;
	struct _cmd_list_cell* next;
	struct _cmd_list_cell* prev;
}cmd_list_cell;

static cmd_list_cell* cmd_list = NULL;

func_ptr find_builtin_cmd(char* name)
{
	cmd_list_cell* aux = cmd_list;
	func_ptr func = NULL;
	while(aux != NULL && func == NULL)
	{
		if(strcmp(name, aux->cmd.name) == 0)
			func = aux->cmd.func;
		aux = aux->next;	
	}
	
	return func;
}

void add_builtin_cmd(func_ptr func, char* name)
{
	if(find_builtin_cmd(name) == NULL) // Si la commande n'existe pas déja on l'ajoute
	{
		if(cmd_list == NULL)
		{
			cmd_list = (cmd_list_cell*) malloc(sizeof(cmd_list_cell));
			cmd_list->next = NULL;
		}
		else
		{
			cmd_list->prev = (cmd_list_cell*)malloc(sizeof(cmd_list_cell));
			cmd_list->prev->next = cmd_list;
			cmd_list = cmd_list->prev;
		}
		
		cmd_list->prev = NULL;
		cmd_list->cmd.func = func;
		cmd_list->cmd.name = strdup(name);
	}
}

int exec_builtin_cmd(char* name)
{
	func_ptr func = find_builtin_cmd(name);
	int ret = -1;
	if(func != NULL)
	{
		ret = 0;
		func();
	}
	return ret;
}

void show_builtin_cmd()
{
	cmd_list_cell* aux = cmd_list;
	printf("Liste des commandes du shell:\n");
	while(aux->next != NULL)
	{
		printf("%s, ",aux->cmd.name);
		aux = aux->next;	
	}
	printf(" %s.\n",aux->cmd.name);
}
