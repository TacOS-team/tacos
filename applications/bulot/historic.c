#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "historic.h"

void init_hist()
{
	shell.my_hist = NULL;
	shell.actual_ptr = NULL;
	shell.last_hist = NULL;
	shell.chaine_vide = '\0';
}

void delete_hist()
{
	historic * to_delete;

	shell.actual_ptr = shell.my_hist;
	if (shell.my_hist != NULL) { /* vérifie que l'historique n'est pas déjà vide */
		while ((shell.actual_ptr->hist_next) != NULL) { /* tant qu'il reste des commandes dans l'historique */
			to_delete = shell.actual_ptr;
			shell.actual_ptr = shell.actual_ptr->hist_next;
			free(to_delete);
		}
		free(shell.actual_ptr); /* vide la dernière commande de l'historique (la plus récente) */
	}

	init_hist(); /* on revient à l'état initial */
}

void insert_new_cmd(char * cmd)
{
	if (shell.my_hist == NULL) { /* cas où l'historique est vide */
		shell.my_hist = malloc(sizeof(historic));
		shell.my_hist->command = malloc((strlen(cmd) + 1) * sizeof(char));
		strncpy(shell.my_hist->command, cmd, strlen(cmd) + 1);
		/* strncpy(shell.my_hist->command, cmd, strlen(cmd) - 1); */
		/* strncat(shell.my_hist->command, "\0", 1); */
		shell.my_hist->num = 1;
		shell.my_hist->hist_next = NULL;
		shell.my_hist->hist_prev = NULL;
		shell.last_hist = shell.my_hist;
		shell.actual_ptr = NULL; /* on ne pointe sur rien actuellement */
	} else { /* cas où l'historique n'est pas vide */
		shell.last_hist->hist_next = malloc(sizeof(historic));
		shell.last_hist->hist_next->hist_prev = shell.last_hist;
		shell.last_hist = shell.last_hist->hist_next;
		shell.last_hist->command = malloc((strlen(cmd) + 1) * sizeof(char));
		strncpy(shell.last_hist->command, cmd, strlen(cmd) + 1);
		/* strncpy(shell.my_hist->command, cmd, strlen(cmd) - 1); */
		/* strncat(shell.my_hist->command, "\0", 1); */
		shell.last_hist->num = shell.last_hist->hist_prev->num + 1;
		shell.last_hist->hist_next = NULL;
		shell.actual_ptr = NULL; /* on ne pointe sur rien actuellement */
	}
	/* printf("append: |%s|\n", shell.my_hist->command); */
}

char * get_current_cmd()
{
	if (shell.my_hist != NULL){	/* vérifie que l'historique n'est pas déjà vide */
		if (shell.actual_ptr == NULL){
			return &(shell.chaine_vide);
		} else {
			return shell.actual_ptr->command;
		}
	} else {
		return &(shell.chaine_vide);
	}
}

char * get_next_cmd()
{
	if (shell.my_hist != NULL) { /* vérifie que l'historique n'est pas déjà vide */
		if (shell.actual_ptr == NULL) {
			return &(shell.chaine_vide);
		} else {
			if ((shell.actual_ptr->hist_next) == NULL) { /* s'il n'y a pas de commandes suivantes */
				return &(shell.chaine_vide);
			} else {
				shell.actual_ptr = shell.actual_ptr->hist_next;
				return shell.actual_ptr->command;
			}
		}
	} else {
		return &(shell.chaine_vide);
	}
}

char * get_prev_cmd()
{
	if (shell.my_hist != NULL) { /* vérifie que l'historique n'est pas déjà vide */
		if (shell.actual_ptr == NULL) {
			shell.actual_ptr = shell.last_hist;
			return shell.actual_ptr->command;
		} else {
			if ((shell.actual_ptr->hist_prev) == NULL) { /* s'il n'y a pas de commandes précédentes */
				return shell.actual_ptr->command;
			} else {
				shell.actual_ptr = shell.actual_ptr->hist_prev;
				return shell.actual_ptr->command;
			}
		}
	} else {
		return &(shell.chaine_vide);
	}
}

void reset_actual_ptr()
{
	shell.actual_ptr=shell.last_hist;
}

int is_empty_hist()
{
	if (shell.my_hist == NULL)
		return 1;
	else
		return 0;
}

void print_hist()
{
	historic * my_ptr;

	my_ptr = shell.my_hist;
	if (shell.my_hist != NULL) { /* vérifie que l'historique n'est pas vide */
		while (my_ptr->hist_next != NULL) {
			printf("  %d  %s\n", my_ptr->num, my_ptr->command);
			my_ptr = my_ptr->hist_next;
		}
	}
}

char **search_cmd_in_hist(char *cmd_in)
{
	int nb_cmd; /* itérateur qui comptera le nombre de commandes trouvées */
	char *my_cmd; /* commande pointée actuellement dans l'historique */
	char **tab_cmd = NULL; /* tableau des commandes trouvées */
	reset_actual_ptr(); /* on se place au début de l'historique */
	if (!is_empty_hist()) { /* si l'historique est vide on ne renvoie rien */
	     while ((my_cmd = get_prev_cmd())) {
			if (my_cmd[0] != '\0') { /* vérifie qu'on n'a pas atteint la fin de l'historique (voir get_prev_cmd() pour plus d'infos) */
				if (strstr(my_cmd, cmd_in) != NULL)
					nb_cmd++; /* compte le nombre de commandes trouvées */
			} else
				break; /* on a fini la recherche */
		}
		tab_cmd = malloc((nb_cmd + 1) * sizeof(char *)); /* alloue la mémoire pour le tableau des commandes trouvées */
		nb_cmd = 0; /* on se replace à 0 pour indicer tab_cmd */
		reset_actual_ptr(); /* on se replace au début de l'historique */
		while ((my_cmd = get_prev_cmd())) {
			if (my_cmd[0] != '\0') { /* vérifie qu'on n'a pas atteint la fin de l'historique (voir get_prev_cmd() pour plus d'infos) */
				if (strstr(my_cmd, cmd_in) != NULL)
					tab_cmd[nb_cmd] = strdup(my_cmd); /* rajoute les commandes trouvées dans tab_cmd */
				nb_cmd++; /* on incrémente l'indice */
			} else
				break; /* on a fini la recherche */
		}
		tab_cmd[nb_cmd] = NULL; /* délimiteur de fin de la liste des commandes trouvées */
	}
	reset_actual_ptr();	/* on se replace au début de l'historique */
	return NULL;
}

