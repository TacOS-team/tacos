/**
 * \file alias.h
 * \brief Gestion des alias pour les commandes du shell
 * Pour utiliser les alias, seulement déclarer une variable de type 
 * my_alias *nom_liste_alias
 * puis l'initialiser avec init_alias()
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef ALIAS_H_
#define ALIAS_H_

#define MAX_LEN 256


/*définition d'une liste circulaire*/
typedef struct alias{
	struct alias *next;
	char name[MAX_LEN];
	char command[2 * MAX_LEN];
} alias;

typedef struct my_alias{
	struct alias *current;
	struct alias *prev;
} my_alias;

/**
 * \fn my_alias *init_alias()
 * \brief Initialise la liste circulaire d'alias et retourne son adresse
 */
my_alias *init_alias();

/**
 * \fn int set_alias(char *name, char *command, my_alias *list_alias)
 * \brief Définit un nouvel alias, s'il existe déja, il est remplacé
 * \return
 * -2 la commande correspondant à l'alias n'as pas pu être def
 * -1 le nom de l'alias n'as pas pu être défini
 * 0 un nouvel alias a été ajouté
 * 1 un alias a été redéfini
 */
int set_alias(char *name, char *command, my_alias *list_alias);

/**
 * \fn int unalias(char *name, my_alias *list_alias)
 * \brief Détruit un alias
 * \return 0 en cas de succes, -1 si l'alias n'existait pas
 */
int unalias(char *name, my_alias *list_alias);
    
/**
 * \fn char *translate(char *command, my_alias *list_alias)
 * \brief Traduit une commande en la valeur de son alias,
 * ATTENTION! pour le moment, l'alias stocke une chaine de char qui doit donc être reparsée par la suite.
 * \return la commande stocker dans l'alias ou fait seulement un echo si cette dernière n'existe pas*/
char *translate(char *command, my_alias *list_alias);

/**
 * \fn void print_aliases(my_alias *list_alias)
 * \brief Affiche tous les alias
 */
void print_aliases(my_alias *list_alias);

/**
 * \fn int insert_alias(char **tab, char *command, int *i, int n);
 * \brief traduit une command en son alias et l'insère dans la table de commande
 */
int insert_alias(char **tab, char *command, int *i, int n);

#endif
