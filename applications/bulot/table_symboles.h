/**
 * \file table_symboles.c
 * \brief Structure de la table des symboles issue de l'analyse Yacc
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef TABLE_SYMBOLES_H_
#define TABLE_SYMBOLES_H_

typedef struct liste_symboles {
	char *nom;
	int valeur;
	char *val_chaine;
	int chaine;
	struct liste_symboles *suivant;
} liste_symboles;

#ifdef VAR_GLOBALES
	liste_symboles *table_symboles;
#else
	extern liste_symboles *table_symboles;
#endif

void inserer_symbole(char *nom, int val, char *str, int ch);
liste_symboles *get_symbole_by_name(char *nom);
void set_val_by_name(char *nom, int val, char *str, int ch);
void vider_table_symboles(liste_symboles *table_symboles);
void print_table_symboles();

#endif /* TABLE_SYMBOLES_H_ */
