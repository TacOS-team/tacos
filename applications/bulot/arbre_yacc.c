/**
 * \file arbre_yacc.c
 * \brief Fonctions nécessaires à l'arbre issu de l'analyse Yacc
 * \author Bul0t Team
 * \date mai 2012
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "jobs.h"

#define VAR_GLOBALES
#include "table_symboles.h"
#include "arbre_yacc.h"
#include "builtcommand.h"
#include "init.h"
#include "historic.h"
#include "customs.h"
#include "wildchar.h"

void exec_tree(int index_pipe)
{
	if (pipe_tab[0] != NULL) {
		create_process(pipe_tab, index_pipe, print_pipe(pipe_tab), infile, outfile, context);
	}
}

void exec_process(char ***tab, int pipe_n, char *l, int inf, int outf, int cont)
{
	if (pipe_tab[0] != NULL) {
		create_process(tab, pipe_n, l, inf, outf, cont);
	}
}

noeud_s* create_const_node(int valeur)
{
	noeud_s *n = malloc(sizeof(*n));
	n->constante_val = valeur;
	n->noeud_t = cste;
	n->branches = NULL;
	return n;
}

noeud_s* create_comm_node(char ***tab, int pipe_n, char *l, int inf, int outf, int cont, char *dir)
{
	noeud_s *n = malloc(sizeof(*n));
	command_s *com = malloc(sizeof(*com));

	com->command_tab = tab;
	com->pipe_num = pipe_n;
	com->line = l;
	com->infile = inf;
	com->outfile = outf;
	com->context = cont;

	n->command = com;
	n->noeud_t = comm;
	n->directory = dir;

	n->branches = NULL;
	return n;
}

noeud_s* create_var_node(char* variable)
{
	noeud_s *n = malloc(sizeof(*n));
	n->var = variable;
	n->noeud_t = var;
	n->branches = NULL;
	return n;
}

noeud_s* create_bu_node(char *oplo, bu_t bl, int bc, char *br, char *br2)
{
	noeud_s *n = malloc(sizeof(*n));

	n->op = oplo;
	n->noeud_t = bu;
	n->builtin_name = bl;
	n->arg1 = br;
	n->arg2 = bc;
	n->arg3 = br2;
	n->branches = NULL;

	return n;
}

noeud_s* create_op_node(char *oplo, noeud_s *bl, noeud_s *bc, noeud_s *br)
{
	
	noeud_s *n = malloc(sizeof(*n));

	n->op = oplo;
	n->noeud_t = op;
	n->branches = malloc(3 * sizeof(noeud_s));

	n->branches[0] = bl;
	n->branches[1] = bc;
	n->branches[2] = br;
	return n;
}

int parse_tree(noeud_s *n)
{
	liste_symboles *symbole;
	int valeur;
	char *val;
	int chaine;
	char ***sauv_pipe;

	if (n != NULL) {
		switch(n->noeud_t) {
		case cste_text:
		     break;
		case op:
			if	(!strcmp(n->op,";")) {
				parse_tree(n->branches[0]);
				parse_tree(n->branches[2]);
			} else if (!strcmp(n->op,"if")) {
				if (parse_tree(n->branches[0]))
					parse_tree(n->branches[1]);
				else
					parse_tree(n->branches[2]);
			} else if (!strcmp(n->op,"==")) {
				if (parse_tree(n->branches[0]) == parse_tree(n->branches[2]))
					return 1;
				else
					return 0;
			} else if (!strcmp(n->op,"&&")) {
				if (parse_tree(n->branches[0]) && parse_tree(n->branches[2]))
					return 1;
				else
					return 0;
			} else if (!strcmp(n->op,"||")) {
				if (parse_tree(n->branches[0]) || parse_tree(n->branches[2]))
					return 1;
				else
					return 0;
			} else if (!strcmp(n->op,"s;")) {
				parse_tree(n->branches[0]);
				parse_tree(n->branches[2]);
			} else if (!strcmp(n->op,"<")) {
				if (parse_tree(n->branches[0]) < parse_tree(n->branches[2]))
					return 1;
				else
					return 0;
			} else if (!strcmp(n->op,">")) {
				if (parse_tree(n->branches[0]) > parse_tree(n->branches[2]))
					return 1;
				else
					return 0;
			} else if (!strcmp(n->op,"!=")) {
				if (parse_tree(n->branches[0]) != parse_tree(n->branches[2]))
					return 1;
				else
					return 0;
			} else if (!strcmp(n->op,"+")) {
				return (parse_tree(n->branches[0]) + parse_tree(n->branches[2]));
			} else if (!strcmp(n->op,"-")) {
				return (parse_tree(n->branches[0]) - parse_tree(n->branches[2]));
			} else if (!strcmp(n->op,"*")) {
				return (parse_tree(n->branches[0]) * parse_tree(n->branches[2]));
			} else if (!strcmp(n->op,"/")) {
				return (parse_tree(n->branches[0]) / parse_tree(n->branches[2]));
			} else if (!strcmp(n->op,"=")) {
				if (n->branches[2]->noeud_t == cste || n->branches[2]->noeud_t == op) {
					valeur = parse_tree(n->branches[2]);
					//printf("vallll %d\n",valeur);
					chaine = 0;
					val = 0;
				} else if ((n->branches[2]->noeud_t == var) && (get_symbole_by_name(n->branches[2]->var)) == NULL) {
					val = n->branches[2]->var;
					valeur = 0;
					chaine = 1;
				} else {
					valeur = parse_tree(n->branches[2]);
					//printf("vallll %d\n",valeur);
					chaine = 0;
					val = 0;
				}
				symbole = get_symbole_by_name(n->branches[0]->var);
				if (symbole == NULL) {
					inserer_symbole(n->branches[0]->var, valeur, val, chaine);
				} else {
					//printf("set val %d pour var %s (ch:%d)\n",valeur,n->branches[0]->var,chaine);
					set_val_by_name(n->branches[0]->var, valeur, val, chaine);
				}
			} else if (!strcmp(n->op,"print")) {
				symbole = get_symbole_by_name(n->branches[2]->var);
				if (symbole != NULL) {
					printf("valeur(%d)\n", symbole->valeur);
				} else {
					printf("var %s inconnue\n", n->branches[2]->var);
				}
			} else if (!strcmp(n->op,"while")) {
				while (parse_tree(n->branches[0])) {
					parse_tree(n->branches[1]);
				}
			}
			break;
		case cste:
			return n->constante_val;
			break;
		case var:
			symbole = get_symbole_by_name(n->var);
			if (symbole != NULL) {
				return symbole->valeur;
			} else {
				printf("var %s inconnue\n", n->var);
			}
			break;
		case bu:
			switch (n->builtin_name) {
			case cd1:
			    if(chdir(n->arg1) < 0) printf("Répertoire inconnu\n");
				break;
			case cd2:
				chdir(getenv("HOME"));
				break;
			case ex:
				shell.end_b = 1;
				break;
			case hi:
				print_hist();
				break;
			case ki:
				print_file_90(n->arg1);
				break;
			case sl1:
				sl(open_bulot, 2);
				break;
			case vb:
				shell.verbose_prompt = 1 - shell.verbose_prompt;
				break;
			case fg1:
				continue_job(get_job_by_id(n->arg2), 1);
				break;
			case fg2:
				continue_job(get_job_by_id(0), 1);
				break;
			case bg1:
				continue_job(get_job_by_id(0), 0);
				break;
			case bg2:
				continue_job(get_job_by_id(n->arg2), 0);
				break;
			case jo:
				print_jobs();
				break;
			case al1:
				set_alias(n->arg1, n->arg3 , shell.alias_list);
				break;
			case al2:
				print_aliases(shell.alias_list);
				break;
			case un:
				unalias(n->arg1, shell.alias_list);
				break;
			}
			break;
			case comm:
				sauv_pipe = copy_all(n->command->command_tab);
				//printf("comm1 %s\n",print_pipe(sauv_pipe));
				replace_symbols_in_command(n,sauv_pipe);
				//printf("comm2 %s\n",print_pipe(sauv_pipe));
				exec_process(sauv_pipe,n->command->pipe_num,n->command->line,n->command->infile,n->command->outfile,context);
				break;
		}
	}
	return 0;
}

void test_tree(noeud_s *n)
{
	if (n != NULL && pipe_tab[0] != NULL) {
		if (n->branches != NULL) {
			test_tree(n->branches[0]);
			test_tree(n->branches[1]);
		}
		if (n->noeud_t == comm) {
			printf("commande\n");
		} else if (n->noeud_t == cste)
			printf("constante int %d ", n->constante_val);
		else if (n->noeud_t == var)
			printf("variable %s ", n->var);
		else if (n->noeud_t == op) {
			printf(" %s ", (char *) n->op);
		} else if (n->noeud_t == bu) {
			printf("buuuuuu\n");
		}
		if (n->branches != NULL) {
			test_tree(n->branches[2]);
		}
	}
}

void replace_symbols_in_command(noeud_s *n,char ***p)
{
    int i = 0,j = 0;
    char *pos;
    char tmp[100];
    char tmp1[100];
    char *copie;
    while(p[i] != NULL) {
	while(p[i][j] != NULL) {
	    // si c'est une variable
	    if (p[i][j] != NULL && *p[i][j] == '$') {
	    	if (get_symbole_by_name(p[i][j] + 1) != NULL) {
				if (get_symbole_by_name(p[i][j] + 1)->chaine) {
					p[i][j] = get_symbole_by_name(p[i][j] + 1)->val_chaine;
				} else {
					tmp[0] = '\0';
					sprintf(tmp,"%d",get_symbole_by_name(p[i][j] + 1)->valeur);
					p[i][j] = strdup(tmp);
				}

				if (p[i][j+1] != NULL && *p[i][j+1] == '.') {
					copie = malloc(LENGTH_LINE);
					strcpy(copie,p[i][j]);
					tmp1[0] = '\0';
					sprintf(tmp1,"%d",get_symbole_by_name(p[i][j+2] + 1)->valeur);
					p[i][j] = strcat(copie,(get_symbole_by_name(p[i][j+2] + 1)->chaine) ? get_symbole_by_name(p[i][j+2] + 1)->val_chaine : tmp1);
					p[i][j+1] = NULL;
					j=j+2;
				}
	    	} else {
	    		p[i][j] = strdup(" ");
	    	}
	    } else if (*p[i][j] == '#') {
		j--;
		copie = malloc(LENGTH_LINE);
		strcpy(copie,p[i][j+1]+1);
		if (n->directory == NULL) {
		    n->directory = default_dir;
		    insert_alias(p[i],p[i][j],&j,90);
		} else {
		    dir = p[i][j-1];
		}
		traduce(copie,n->directory,p[i],&j,90);
		p[i][j] = NULL;
		j--;
	    } else if ((pos = strchr(p[i][j],'\\')) != NULL) {
		do {
		    while (*pos != '\0') {
			*pos = *(pos + 1);
			pos++;
		    }

		} while((pos = strchr(p[i][j],'\\')) != NULL);
	    } else if (*p[i][j] == '`') {
		printf("loooool");
	    }
	    j++;
	}
	j = 0; i++;
    }
}

char **copy_tab(char **tab)
{
	int i = 0;
	char ** copy;
	while (tab[i] != NULL) {
		i++;
	}
	copy = malloc(sizeof(*copy) * (i + MAX_ARG));
	i = 0;
	while (tab[i] != NULL) {
		copy[i] = tab[i];
		i++;
	}
	copy[i] = NULL;
	return copy;
}

char ***copy_pipe(char ***tab)
{
	int i = 0;
	char *** copy;
	while (tab[i] != NULL) {
		i++;
	}
	copy = malloc(sizeof(*copy) * (i + 1));
	i = 0;
	while (tab[i] != NULL) {
		copy[i] = tab[i];
		i++;
	}
	copy[i] = NULL;
	return copy;
}

char ***copy_all(char ***tab)
{
	int i = 0;
	char *** copy;
	while (tab[i] != NULL) {
		i++;
	}
	copy = malloc(sizeof(*copy) * (i + 1));
	i = 0;
	while (tab[i] != NULL) {
		copy[i] = copy_tab(tab[i]);
		i++;
	}
	copy[i] = NULL;
	return copy;
}

void print_tab(char **tab)
{
	int i = 0;
	while (tab[i] != NULL) {
		printf("%s\n",tab[i]);
		i++;
	}
}

char *print_pipe(char ***tab)
{
	int i = 0, j = 0, k = 0, index = 0;
	char sortie[100000];
	while (tab[i] != NULL) {
		(i >= 1) ? sortie[index++] = '|' : 0;
		(i >= 1) ? sortie[index++] = ' ' : 0;
		while (tab[i][j] != NULL) {
			while (tab[i][j][k] != '\0') {
				sortie[index] = tab[i][j][k];
				index++;
				k++;
			}
			j++;
			k = 0;
			sortie[index++] = ' ';
		}
		i++;
		j = 0;
	}
	sortie[index] = '\0';
	return strdup(sortie);
}


void free_commande(command_s* commande)
{
	int i, j;
	for (i = 0; commande->command_tab != NULL; i++) {
		for (j = 0; commande->command_tab[i] != NULL; j++) {
			cfree(commande->command_tab[i][j]);
		}
		cfree(commande->command_tab[i]);
	}
	cfree(commande->command_tab);
	cfree(commande->line);
}


void vider_arbre(noeud_s *arbre)
{
	int i = 0;
	//printf("vider_arbre(%p)\n", arbre);
	if (arbre != NULL) {
		if (arbre->branches != NULL) {
			while (arbre->branches[i] != NULL) {
				//printf("branche %d\n", i);
				vider_arbre(arbre->branches[i]);
				i++;
			}
		}
		switch (arbre->noeud_t) {
		case comm:
			//printf("comm\n");
			free_commande(arbre->command);
			break;
		case var:
			//printf("var\n");
			if (arbre->var != NULL) {
				cfree(arbre->var);
			}
			break;
		case op:
			//printf("op : %s\n", arbre->op);
			if (strcmp(arbre->op, ";") != 0) {
				cfree(arbre->op);
			}
			break;
		case cste:
			break;
		case cste_text:
			break;
		case bu:
			break;
		}
		if (arbre->directory != NULL) {
			cfree(arbre->directory);
		}
		cfree(arbre);
		//printf("branche vidée\n");
	}
}


void vider_liste_arbres(liste_arbres *arbres)
{
	//printf("vider_liste_arbres(%p)\n", arbres);
	if (arbres != NULL) {
		if (arbres->suivant != NULL) {
			vider_liste_arbres(arbres->suivant);
		}
		vider_arbre(arbres->arbre);
		/*printf("noeuds arbre vidés\n");
		printf("%p, %p\n", arbres->arbre, arbres->suivant);*/
		free(arbres);

	}
	//printf("arbre vidé\n\n");
}

void inserer_arbre(noeud_s *arbre)
{
	/*printf("inserer_arbre\n");
	printf("avant\n");
	print_liste_arbres();*/
	liste_arbres *aux = arbres;
	if (aux != NULL) {
		while (aux->suivant != NULL) {
			aux = aux->suivant;
		}
		aux->suivant = (liste_arbres *)cmalloc(sizeof(liste_arbres));
		aux = aux->suivant;
		aux->arbre = arbre;
		aux->suivant = NULL;
	} else {
		arbres = (liste_arbres *)cmalloc(sizeof(liste_arbres));
		arbres->arbre = arbre;
		arbres->suivant = NULL;
	}
	/*printf("après\n");
	print_liste_arbres();
	printf("arbre inséré\n");*/
}

void supprimer_arbre(noeud_s *arbre)
{
	liste_arbres *aux = arbres;
	noeud_s *aux2;
	if (aux != NULL) {
	  while (((noeud_s *)aux->suivant != NULL) && ((noeud_s *)aux->suivant != (noeud_s *)arbre)) {
			aux = aux->suivant;
		}
		if (aux->suivant != NULL) {
			aux2 = (aux->suivant)->arbre;
			aux->suivant = (aux->suivant)->suivant;
			vider_arbre(aux2);
		}
	}
}

void print_liste_arbres()
{
	int i = 0;
	liste_arbres *aux = arbres;
	while (aux != NULL) {
		printf("%d : %p\n", i, aux);
		i++;
		aux = aux->suivant;
	}
}
