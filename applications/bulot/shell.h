/**
 * \file shell.h
 * \brief Structures de données pour le shell
 * \author Bul0t Team
 * \date mai 2012
 */

#ifndef SHELL_H_
#define SHELL_H_


#include <termios.h>
#include <unistd.h>
#include <sys/types.h>

#include "const.h"
#include "alias.h"

typedef struct historic{	/* structure qui contiendra l'historique, basée sur une liste doublement chaînée non circulaire */
	int num;
	char * command;
	struct historic * hist_next;
	struct historic * hist_prev;
} historic;

typedef struct process {
  struct process *next;
  char **argv;
  pid_t pid;
  char completed;
  char stopped;
  int status;
} process;

typedef struct job {
  struct job *next;
  char *command;
  process *first_process;
  pid_t pgid;
  char notified;
  struct termios tmodes;
  int stdin, stdout, stderr;
} job;

typedef struct token {
  char **tokens;
  int size;
} token;

typedef struct shell_struct {
    pid_t shell_pgid;
    int shell_terminal;
    int shell_is_interactive;
    struct termios shell_tmodes;
    
    my_alias *alias_list; /* pointeur vers sur la liste des alias*/
    historic * my_hist;	/* pointeur vers l'historique (haut de l'historique) */
    historic * actual_ptr;	/* pointeur sur une commande de l'historique */
    historic * last_hist;	/* pointeur vers la dernière commande tapée */
    char chaine_vide;
    char save_buf[1024];/*buffer de copie*/
    char buf[1024];/*buffer*/
    
    int end_b;          /* Indique si le shell est arrété ou
			 * non (depuis exit::builtcommand) */

    int verbose_prompt;	/* booléen (initialisé à 1) indiquant si le prompt
			 * affiche "currentDirectory", commande built-in
			 * "verboseprompt" ou "vbpt" pour changer sa
			 *  valeur */
     int term_owner; 
     int script_mode;
     
  FILE *fichier_script;

} shell_struct;

extern shell_struct shell;

#endif /* SHELL_H_ */
