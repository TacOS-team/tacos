#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include "historic.h"
#include "term.h"
#include "autocomp.h"
#include "init.h"
#include "shell.h"
#include "alias.h"

#define KTAB 9
#define BLANK 32
#define KUP 454081
#define KDOWN 454082
#define KRIGHT 454083
#define KLEFT 454084
#define KPGUP 58120958
#define KPGDOWN 58121086
#define KDEL 58120702

/*regroupe les variables pour la gestion de la tabulation*/
struct tab
{
	int tab;
	int noccur;
	char *lw;
	char *suggest;
	int save_pos;
	int after_com; /*vaut 1 si on est apres une commande*/
};

void move_left(int *pos, int *cur_col, int colonnes)
{
	if (*pos > 0) {
		(*pos)--;
		if (*cur_col > 0) {
			printf("\033[D");
			(*cur_col)--;
		} else {
			printf("\033[A\033[%dC", colonnes - 1);
			*cur_col = colonnes - 1;
		}
	}
}

void move_right(int *pos, int *cur_col, int colonnes, int count)
{
	if (*pos < count) {
		(*pos)++;
		if (*cur_col < colonnes - 1) {
			printf("\033[C");
			(*cur_col)++;
		} else {
			printf("\033[B\033[%dD", colonnes - 1);
			*cur_col = 0;
		}
	}
}

/*attention, del_char ne place pas correctement le curseur, il faut le coupler avec des move_left/right lors de son utilisation*/
void del_char(int *pos, int *cur_col, int colonnes, int *count, shell_struct *shell)
{

	int t,m,i;

	if ((*pos) == (*count)) {
		return;
	}
	for (i = *pos; i < *count; i++) {
		shell->buf[i] = shell->buf[i + 1];
	}

	(*count)--;

	printf("%s ", &shell->buf[(*pos)]);

	t = *count - *pos + 1;
	m = (t + *cur_col - 1) / colonnes;
	if (m > 0) {
		printf("\033[%dA", m);
	}

	t -= m * colonnes;
	if (t > 0) {
		printf("\033[%dD", t);
	} else if (t < 0) {
		printf("\033[%dC", -t);
	}
	return;
}

void insert_char(int *pos, int *cur_col, int colonnes, int *count, shell_struct *shell, int c)
{
	int t,m,i;

	if (c >= 32 && c < 128) {

		/* Insert mode.*/
		for (i = *count; i >= *pos; i--) {
			shell->buf[i + 1] = shell->buf[i];
		}
		(*count)++;

		shell->buf[(*pos)++] = c;
		if (*pos < *count) {
			printf("%s", shell->buf + *pos - 1);

			t = *count - *pos + 1;
			m = (t + *cur_col - 1) / colonnes;
			if (m > 0) {
				printf("\033[%dA", m);
			}

			t -= m * colonnes;
			if (t > 0) {
				printf("\033[%dD", t);
			} else if (t < 0) {
				printf("\033[%dC", -t);
			}

			if (*cur_col < colonnes - 1) {
				printf("\033[C");
				(*cur_col)++;
			} else {
				printf("\033[B\033[%dD", colonnes - 1);
				*cur_col = 0;
			}
		} else {
			printf("%c", c);

			if (*cur_col < colonnes - 1) {
				(*cur_col)++;
			} else {
				*cur_col = 0;
				printf(" \033[D");
			}
		}
	}
}
/*fonction d'initialisation de la structure pour la gestion de tab*/
void init_tab(struct tab *t){
	t->tab = 0;
	t->noccur = 0;
	t->lw = NULL;
	t->suggest = NULL;
	t->save_pos = 0;
	t->after_com = 0;
}

static int getbigchar()
{
	int r = getchar();
	long flags;
	if ((flags = fcntl(0, F_GETFL, (void *)0)) == -1) {
		flags = 0;
	}
	fcntl(0, F_SETFL, (void *)(flags | O_NONBLOCK));
	int c;
	while ((c = getchar()) != -1)
		r = r * 128 + c;
	fcntl(0, F_SETFL, (void *)flags);

	return r;
}

void readline(shell_struct *shell)
{
	int i;
	int end = 0; /*stoppe la saisie lorsqu'il vaut 1*/
	int cur_col = 0; /*dans le terminal, indique la position du pointeur*/
	struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	int colonnes = ws.ws_col;


	cur_col = shellPrompt(0);
	fflush(stdout);
	printf("\033[s");

	struct termios oldt;
	if (tcgetattr(0, &oldt) != 0) {
		perror("Erreur tcget");
	}
	struct termios newt = oldt;
	cfmakeraw(&newt);
	/* On change la structure termios default, on ajout un retour chariot a chaque newline detecté */
	newt.c_oflag |= (ONLCR | OPOST);
	if (tcsetattr(0, TCSANOW, &newt) != 0) {
		perror("Erreur tcset");
	}

	char *com_hist;
	int count = 0;/*sauvegarde le nombre de char dans le buffer*/
	int pos = 0;/*position du curseur dans le buffer buf*/

	/*variables pour la tabulation*/
	struct tab tabu;
	init_tab(&tabu);

	shell->buf[count] = '\0';

	do {
		int t, m;
		int c = getbigchar();
		switch (c) {
		case 1: /*C^a*/
			while (pos > 0) {
				move_left(&pos,&cur_col,colonnes);
			}
			break;
		case 3: /*C^c*/
			shell->buf[0] = '\0';
			break;
		case 4: /*C^D*/
			shell->end_b = 1;
			end = 1;
			shell->buf[0] = '\n';
			break;
		case 5: /*C^e*/
			while (pos != count) {
				move_right(&pos,&cur_col,colonnes,count);
			}
			break;
		case 9:/*TAB autocompletion */
			tabu.tab = 1;
			/*tab est pressé pour la première fois*/
			if (tabu.noccur == 0){
				/*on récupère le dernier mot*/
				tabu.lw = lastword(shell->buf, &(shell->buf[pos]), &(tabu.after_com), ' ');
				/*on sauvegarde la position*/
				tabu.save_pos = pos - strlen(tabu.lw);
			}
			/*on efface*/
			while( pos > tabu.save_pos){
				move_left(&pos,&cur_col,colonnes);
				del_char(&pos, &cur_col, colonnes, &count, shell);
			}
			++tabu.noccur;
			tabu.suggest = look_up_command(tabu.lw, tabu.noccur, tabu.after_com);
			if (!strncmp(tabu.suggest, "",1)){
				tabu.noccur = 1;
				tabu.suggest = tabu.lw; /*on reprend le mot de base*/
			}
			/*on insère*/
			i = 0;
			while (tabu.suggest[i] != '\0') {
				insert_char(&pos, &cur_col, colonnes, &count, shell, tabu.suggest[i]);
				i++;
			}
			break;
		case 11:/*^Ck*/
			strncpy(shell->save_buf, &shell->buf[pos], count - pos);
			shell->save_buf[(count + 1) - pos] = '\0';
			while(pos != count) {
				del_char(&pos,&cur_col,colonnes,&count,shell);
			}
			break;
		case 10:/*Enter*/
			end = 1;
			while (pos != count) {
				move_right(&pos,&cur_col,colonnes,count);
			}
			strncmp(shell->buf, "\0", 1) ? insert_new_cmd(shell->buf) : 0;
			shell->buf[pos] = '\0';
			break;
		case 25:/*C^y*/
			i = 0;
			while (shell->save_buf[i] != '\0') {
				insert_char(&pos, &cur_col, colonnes, &count, shell, shell->save_buf[i]);
				i++;
			}
			break;
		case 2: /*C^b*/
		case KLEFT:
			move_left(&pos, &cur_col, colonnes);
			break;
		case 6:/*C^f*/
		case KRIGHT:
			move_right(&pos, &cur_col, colonnes, count);
			break;
		case KUP:/*historique -*/
			if (!is_empty_hist()){
				printf("\033[u");
				printf("\033[K");
				cur_col = shellPrompt(0);
				com_hist = get_prev_cmd();
				printf("%s", com_hist);
				strncpy(shell->buf, com_hist, strlen(com_hist) + 1);
				cur_col += strlen(com_hist);
				count = strlen(com_hist);
				pos = count;
			}
			break;
		case KDOWN:/*historique +*/
			if (!is_empty_hist()){
				printf("\033[u");
				printf("\033[K");
				cur_col = shellPrompt(0);
				com_hist = get_next_cmd();
				printf("%s", com_hist);
				strncpy(shell->buf, com_hist, strlen(com_hist) + 1);
				cur_col += strlen(com_hist);
				count = strlen(com_hist);
				pos = count;
			}
			break;
		case KDEL: /* Touche suppr */
			del_char(&pos,&cur_col,colonnes,&count,shell);
			break;
		case 127: /* Touche retour (del) */
			tabu.noccur = 0;
			if (pos == 0) {
				break;
			}
			if (pos < count) {
				for (i = pos - 1; i < count; i++) {
					shell->buf[i] = shell->buf[i + 1];
				}
			} else {
				shell->buf[pos - 1] = '\0';
			}
			if (cur_col > 0) {
				printf("\033[D");
				cur_col--;
			} else {
				printf("\033[A\033[%dC", colonnes - 1);
				cur_col = colonnes - 1;
			}
			count--;
			pos--;
			printf("%s ", &shell->buf[pos]);

			t = count - pos + 1;
			m = (t + cur_col - 1) / colonnes;
			if (m > 0) {
				printf("\033[%dA", m);
			}

			t -= m * colonnes;
			if (t > 0) {
				printf("\033[%dD", t);
			} else if (t < 0) {
				printf("\033[%dC", -t);
			}

			break;
		default:
			if (tabu.tab){
				tabu.noccur = 0;
			}
			insert_char(&pos, &cur_col, colonnes, &count, shell, c);
			break;
		}
		fflush(stdout);
	} while (!end);

	if (tcsetattr(0, TCSANOW, &oldt) != 0) {
		perror("erreur tcset");
	}
	printf("\n");
}
