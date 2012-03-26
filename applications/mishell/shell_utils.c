/**
 * @file shell_utils.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
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

int exec_builtin_cmd(char* cmd)
{
	char * name = strdup(cmd);
	int argc = 0;
	char **argv = malloc(100 * sizeof(char*));
	char *s;
	s = strtok(name, " ");
	while (s != NULL) {
		argv[argc++] = s;
		s = strtok(NULL, " ");
	}

	func_ptr func = find_builtin_cmd(argv[0]);
	int ret = -1;
	if(func != NULL)
	{
		char *buf = malloc(10);
		sprintf(buf, "?=%d", func(argc, argv));
		putenv(buf);
		ret = 0;
	}
	return ret;
}

void show_builtin_cmd()
{
	cmd_list_cell* aux = cmd_list;
	printf("Liste des commandes du shell:\n\n");
	int i = 1;
	while(aux != NULL) {
		if (i == 0)	{
			i = 1;
			printf("%s\n",aux->cmd.name);
		} else {
			i = (i+1)%3;
			printf(" %s\t",aux->cmd.name);
			if(strlen(aux->cmd.name) < 7) {
				printf("\t");
			}
		}
		aux = aux->next;	
	}
	printf("\n");
}
