#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "customs.h"
#include "alias.h"
#include "shell.h"

/*initialise la liste circulaire d'alias et retourne son adresse*/
my_alias *init_alias()
{
	my_alias *list_alias = cmalloc(sizeof(my_alias));

	list_alias->current = NULL;
	list_alias->prev = list_alias->current;

	return list_alias;
}

/*fonction interne*/
int alias_is_empty(my_alias *list_alias) {
	return list_alias->current == NULL;
}

/*retourne l'adresse d'un alias si il est trouvé, null sinon*/
alias *find_alias(char *name, my_alias *list_alias)
{
	alias *out = NULL;
	size_t n;
	int found = 0;
	alias *start;

	if (!alias_is_empty(list_alias)) {
		if (name != NULL) {
			n = strlen(name);
			start = list_alias->current;
			do {
				found = ((n == strlen(list_alias->current->name) && !strncmp(name, list_alias->current->name, n)));
				if (!found) {
					list_alias->prev = list_alias->current;
					list_alias->current = list_alias->current->next;
				}
			} while (list_alias->current != start && !found);
			if (found)
				out = list_alias->current;
		}
	}

	return out;
}

/*def un nouvel alias et le met dans la liste, si le nom existe deja,
 *il est remplacé
 */
int set_alias(char *name, char *command, my_alias *list_alias)
{
	int out = 1;
	char *nametmp = strndup(name, strlen(name) - 1);
	alias *new = find_alias(nametmp, list_alias);
	int created = 0;

	if (new == NULL) {
		new = cmalloc(sizeof(alias));
		created = 1;
	}
	/*remplissage de l'alias*/
	if (strlen(nametmp) < sizeof(new->name)) {
		strncpy(new->name, nametmp, strlen(nametmp) + 1);
	} else {
		out = -1;
	}
	if (strlen(command) < sizeof(new->command)) {
		strncpy(new->command, command, strlen(command) + 1);
	} else {
		out = -2;
	}

	if (created) {
		/*insertion dans la liste d'alias*/
		if (alias_is_empty(list_alias)) {
			list_alias->current = new;
			list_alias->prev = new;
			new->next = new;
		} else {
			list_alias->prev = list_alias->current;
			new->next = list_alias->current->next;
			list_alias->current->next = new;
			list_alias->current = new;
		}
		out = 0;
	}
	cfree(nametmp);
	return out;
}


/*retourn 0 en cas de succes, -1 si l'alias n'existait pas*/
int unalias(char *name, my_alias *list_alias)
{
	alias *to_delete = find_alias(name, list_alias);
	int out = 0;

	if (to_delete != NULL) {
		/*si l'alias était le seul enregistré*/
		if (to_delete->next == to_delete) {
			list_alias->current = NULL;
			list_alias->prev = NULL;
		} else {
			list_alias->current = to_delete->next;
			list_alias->prev->next = list_alias->current;
		}
		cfree(to_delete);
	} else {
		printf("alias %s n'existe pas\n", name);
		out = -1;
	}

	return out;
}

/*traduit une commande en la valeur de son alias, ATTENTION! pour le
 * moment, l'alias stocke une chaine de char qui doit donc être
 * reparsée par la suite.
 *retourne la commande stockée dans l'alias ou fait seulement un echo
 *si cette dernière n'existe pas
 */
char *translate(char *command, my_alias *list_alias)
{
	alias *to_translate;
	char *translated;

	if ((to_translate = find_alias(command, list_alias)) == NULL) {
		translated = command;
	} else {
		translated = to_translate->command;
	}

	return translated;
}


/*affiche tous les alias*/
void print_aliases(my_alias *list_alias)
{
	alias *start;

	if (alias_is_empty(list_alias)) {
		printf("No alias defined\n");
	} else {
		start = list_alias->current;
		do {
			printf("alias %s=%s\n", list_alias->current->name, list_alias->current->command);
			list_alias->prev = list_alias->current;
			list_alias->current = list_alias->current->next;
		} while (list_alias->current != start);
	}
}


/*
 *s'occupe de la traduction d'une commande en son alias ainsi que dans
 *son insertion dans la table des commandes
 */
int insert_alias(char **tab, char *command, int *i, int n){    
/*on récupère l'alias (ou bien la commande elle meme si pas d'alias def)*/
	char *new_command = translate(command, shell.alias_list);
	//printf("alias = [%s]\n", new_command);
	char *token = cmalloc((strlen(new_command) + 1) * sizeof(char));
	char *s = cmalloc((strlen(new_command) + 1) * sizeof(char));
	strncpy(s, new_command, strlen(new_command) + 1);
	char **ptab = tab;
/*on découpe l'alias et on l'insère dans la table*/
	token = strtok(s, " ");
	while (token!=NULL){
		ptab = ptab + *i;
		//printf("copie de %s\n", token);
		*ptab = strdup(token);
		*i = *i + 1;
		token = strtok(NULL, " ");
	}
	return 0;
}
