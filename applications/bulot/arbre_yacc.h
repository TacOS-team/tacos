/**
 * \file arbre_yacc.h
 * \brief Structure de l'arbre issu de l'analyse Yacc
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef ARBREYACC_H_
#define ARBREYACC_H_


#define TAILLE_SYMBOLE 30

typedef enum {comm, var, op, cste, cste_text, bu} enumtype;
typedef enum {cd1,cd2,ex,hi,ki,sl1,vb,fg1,fg2,bg1,bg2,jo,al1,al2,un} bu_t;

typedef struct noeud {

	enumtype noeud_t;
	union {
		int variable;			/* Si le noeud est une variable, sa position dans le tableau des variables */
		char *var;				/* nom de la variable */
		int constante_val;		/* Si le noeud est une constante, sa valeur */
		char *op;				/* on stocke la chaine correspondant à l'opérateur */
		bu_t builtin_name;
		struct command_type *command;
	};
	char *arg1; 			/* Pour les builtin */
	int arg2; 				/* Pour les builtin */
	char *arg3; 			/* Pour les builtin */

	char *directory; 		/* Pour l'auto-complétion */
	struct noeud **branches; /* Les branches de l'operateur */

} noeud_s;

typedef struct command_type {
	char ***command_tab;
	int pipe_num;
	char *line;
	int infile;
	int outfile;
	int context;
} command_s;


typedef struct liste_arbres {
	noeud_s *arbre;					/* racine d'un arbre */
	struct liste_arbres *suivant;	/* autres arbres */
} liste_arbres;

#ifdef VAR_GLOBALES
	liste_arbres *arbres;
	char **pipe_tab[10];
	int context = 0;
	int infile = 0;
	int outfile = 0;
	int builtin = 0;

	/*pour les wildchars*/
	char *tmp;
	char *dir;
	char default_dir[] = "./";
#else
	extern liste_arbres *arbres;
	extern char **pipe_tab[10];
	extern int context;
	extern int infile;
	extern int outfile;
	extern int builtin;

	/*pour les wildchars*/
	extern char *tmp;
	extern char *dir;
	extern char default_dir[];
#endif


void exec_tree(int index_pipe);
void exec_process(char ***tab, int pipe_n, char *l, int inf, int outf, int cont);
noeud_s* create_const_node();
noeud_s* create_var_node();
noeud_s* create_bu_node(char*, bu_t, int, char*, char*);
noeud_s* create_comm_node(char ***tab, int pipe_n, char *l, int inf, int outf, int cont, char *dir);
noeud_s* create_op_node(char*, noeud_s*,noeud_s*,noeud_s*);
int parse_tree(noeud_s *n);
void test_tree(noeud_s *n);
void replace_symbols_in_command(noeud_s *n, char ***p);

//void free_commande(command_s* commande);
void supprimer_arbre();
void vider_arbre(noeud_s *arbre);
void vider_liste_arbres(liste_arbres *arbres);
void inserer_arbre(noeud_s *arbre);
void print_liste_arbres();

char **copy_tab(char **tab);
char ***copy_pipe(char ***tab);
char ***copy_all(char ***tab);

void print_tab(char **tab);
char *print_pipe(char ***tab);


#endif /* ARBREYACC_H_ */
