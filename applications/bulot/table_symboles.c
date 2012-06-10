/**
 * \file table_symboles.c
 * \brief Fonctions nécessaires à la table des symboles issue de l'analyse Yacc
 * \author Bul0t Team
 * \date mai 2012
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VAR_GLOBALES
#include "table_symboles.h"
#include "customs.h"

void inserer_symbole(char *nom, int val, char *str, int ch)
{
	//printf("inserer_symbole(%s, %d, %s, %d)\n", nom, val, str, ch);

	/*printf("avant\n");
	print_table_symboles();*/

	liste_symboles *aux = table_symboles;
	if (aux != NULL) {
		while (aux->suivant != NULL) {
			aux = aux->suivant;
		}
		aux->suivant = (liste_symboles *)cmalloc(sizeof(liste_symboles));
		aux = aux->suivant;
		aux->nom = strdup(nom);
		if (ch) {
			aux->val_chaine = strdup(str);
		} else {
			aux->valeur = val;
			aux->val_chaine = strdup("");
		}
		aux->chaine = ch;
		aux->suivant = NULL;
	} else {
		table_symboles = (liste_symboles *)cmalloc(sizeof(liste_symboles));
		table_symboles->nom = strdup(nom);
		if (ch) {
			table_symboles->val_chaine = strdup(str);
		} else {
			table_symboles->valeur = val;
			table_symboles->val_chaine = strdup("");
		}
		table_symboles->chaine = ch;
		table_symboles->suivant = NULL;
	}

	/*printf("après\n");
	print_table_symboles();*/
}

liste_symboles *get_symbole_by_name(char *nom)
{
	liste_symboles *symbole = NULL;
	liste_symboles *aux = table_symboles;
	while ((aux != NULL) && ((strcmp(aux->nom, nom)) != 0)) {
		//printf("\twhile - aux->nom = %s\n", aux->nom);
		aux = aux->suivant;
	}
	//printf("\taux->nom = %s\n", aux->nom);
	if (aux != NULL) {
		symbole = aux;
		//printf("\taux->nom = %s\n", aux->nom);
	}
	return symbole;
}

void set_val_by_name(char *nom, int val, char *str, int ch)
{
	//printf("set_val_by_name(%s, %d)\n", nom, val);
	liste_symboles *aux = table_symboles;
	while ((aux != NULL) && ((strcmp(aux->nom, nom)) != 0)) {
		//printf("\twhile - aux->nom = %s\n", aux->nom);
		aux = aux->suivant;
	}
	//printf("\taux->nom = %s\n", aux->nom);
	if (aux != NULL) {
		if (ch) {
			aux->val_chaine = str;
		} else {
			aux->valeur = val;
		}
		//printf("\taux->valeur = %d\n", aux->valeur);
		aux->chaine = ch;
	}
}

void vider_table_symboles(liste_symboles *table_symboles)
{
	//printf("vider_table_symboles\n");
	if (table_symboles != NULL) {
		vider_table_symboles(table_symboles->suivant);
		cfree(table_symboles->nom);
		if (table_symboles->val_chaine != NULL) {
			cfree(table_symboles->val_chaine);
		}
		cfree(table_symboles);
	}
}

void print_table_symboles()
{
	int i = 0;
	liste_symboles *aux = table_symboles;
	while (aux != NULL) {
		printf("%d : %p, %d, %p, %d\n", i, aux->nom, aux->valeur, aux->val_chaine, aux->chaine);
		aux = aux->suivant;
		i++;
	}
}
