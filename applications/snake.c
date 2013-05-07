/**
 * @file snake.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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
 * @brief Jeu snake en mode texte.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

static int dir = 2, dir_ = 2;
static int dir_ia = 3;
static int lignes;
static int colonnes;

struct coord_t {
	int l, c;
};

struct snake_t {
	int longueur;
	int indice_deb;
	struct coord_t *coords;
};

static struct snake_t snake;
static struct snake_t snake_ia;
static struct coord_t directions[4] = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };

static int **grid;

#define HAUT 0
#define DROITE 1
#define BAS 2
#define GAUCHE 3

static struct coord_t bonus;

static void haut()
{
	if (dir_ != BAS) {
		dir = HAUT;
	}
}

static void bas()
{
	if (dir_ != HAUT) {
		dir = BAS;
	}
}

static void droite()
{
	if (dir_ != GAUCHE) {
		dir = DROITE;
	}
}

static void gauche()
{
	if (dir_ != DROITE) {
		dir = GAUCHE;
	}
}

void thread_input()
{
	char c;
	int r;
	do {
		c = getchar();
		switch (c) {
		case 'z':
			haut();
			break;
		case 's':
			bas();
			break;
		case 'd':
			droite();
			break;
		case 'q':
			gauche();
			break;
		case 27:
			r = 1;
			while ((c = getchar()) != -1) {
				if (c == 27) {
					r = 1;
					continue;
				}
				r = r * 128 + c;
			}
			switch (r) {
			case 28097:
				haut();
				break;
			case 28098:
				bas();
				break;
			case 28099:
				droite();
				break;
			case 28100:
				gauche();
				break;
			}
		}
	} while (c != EOF);
}

void clear_screen()
{
	printf("\033[2J");
	fflush(stdout);
}

void print_locate(int l, int c, char car)
{
	int fgcolor;
	switch (car) {
	case '1':
		fgcolor = 34;
		car = '*';
		break;
	case '2':
		fgcolor = 31;
		car = '*';
		break;
	case '#':
		fgcolor = 30;
		break;
	case ' ':
		fgcolor = 37;
		break;
	case '+':
		fgcolor = 32;
		break;
	default:
		fgcolor = 30;
	}

	printf("\033[%d;%df\033[%dm\033[107m%c\033[%d;%df", l + 1, c + 1,
	       fgcolor, car, lignes, colonnes);
	fflush(stdout);
}

static inline int collision(int l, int c)
{
	return grid[l][c];
}

static void genere_bonus()
{
	do {
		bonus.l = (rand() % (lignes - 2)) + 2;
		bonus.c = (rand() % (colonnes - 2)) + 2;
	} while (collision(bonus.l, bonus.c));
	print_locate(bonus.l, bonus.c, '+');
}

void init_snake()
{
	// player
	dir = DROITE;
	dir_ = DROITE;
	snake.longueur = 1;
	snake.indice_deb = 0;
	snake.coords[snake.indice_deb].c = colonnes / 3;
	snake.coords[snake.indice_deb].l = lignes / 2;
	print_locate(snake.coords[snake.indice_deb].l,
		     snake.coords[snake.indice_deb].c, '1');
	grid[lignes / 2][colonnes / 3] = 1;

	// ia
	dir_ia = GAUCHE;
	snake_ia.longueur = 1;
	snake_ia.indice_deb = 0;
	snake_ia.coords[snake_ia.indice_deb].c = 2 * colonnes / 3;
	snake_ia.coords[snake_ia.indice_deb].l = lignes / 2;
	print_locate(snake_ia.coords[snake_ia.indice_deb].l,
		     snake_ia.coords[snake_ia.indice_deb].c, '2');
	grid[lignes / 2][2 * colonnes / 3] = 1;

	genere_bonus();
}

static inline int dans_aire_jeu(int l, int c)
{
	return l > 0 && l < lignes - 1 && c > 0 && c < colonnes - 1;
}

int avance_snake()
{
	static int allonge = 0;
	if (snake.longueur < 4 || allonge) {
		snake.longueur++;
		allonge = 0;
	} else {
		print_locate(snake.coords[snake.indice_deb].l,
			     snake.coords[snake.indice_deb].c, ' ');
		grid[snake.coords[snake.indice_deb].l][snake.
						       coords[snake.indice_deb].
						       c] = 0;
		snake.indice_deb = (snake.indice_deb + 1) % (lignes * colonnes);
	}

	int i = (snake.indice_deb + snake.longueur - 1) % (lignes * colonnes);
	int i_ = (snake.indice_deb + snake.longueur - 2) % (lignes * colonnes);
	snake.coords[i].l = snake.coords[i_].l + directions[dir].l;
	snake.coords[i].c = snake.coords[i_].c + directions[dir].c;

	dir_ = dir;

	if (snake.coords[i].l == bonus.l && snake.coords[i].c == bonus.c) {
		genere_bonus();
		allonge = 1;
	}

	if (!dans_aire_jeu(snake.coords[i].l, snake.coords[i].c)
	    || collision(snake.coords[i].l, snake.coords[i].c)) {
		return -1;
	}

	print_locate(snake.coords[i].l, snake.coords[i].c, '1');
	grid[snake.coords[i].l][snake.coords[i].c] = 1;
	return 0;
}

struct noeud_t {
	int dir;
	struct coord_t c;
};

struct coord_t get_direction(struct coord_t *c, int dir)
{
	struct coord_t r;
	r.l = c->l + directions[dir].l;
	r.c = c->c + directions[dir].c;
	return r;
}

int pcc(struct coord_t *orig, struct coord_t *dest)
{
	struct noeud_t *Q = malloc(lignes * colonnes * sizeof(struct noeud_t));
	int **grid_visited = malloc(sizeof(int *) * lignes);
	int l;
	for (l = 0; l < lignes; l++) {
		grid_visited[l] = malloc(sizeof(int) * colonnes);
		memcpy(grid_visited[l], grid[l], sizeof(int) * colonnes);
	}

	int n = 0;
	int r = 0;
	int b = 0;

	grid_visited[orig->l][orig->c] = 1;
	Q[n].dir = -1;
	Q[n].c = *orig;
	n++;

	while (b != n) {
		struct noeud_t noeud = Q[b];
		b = (b + 1) % (lignes * colonnes);

		if (noeud.c.l == dest->l && noeud.c.c == dest->c) {
			r = noeud.dir;
			break;
		}

		int d;
		for (d = 0; d < 4; d++) {
			struct coord_t cc = get_direction(&(noeud.c), d);

			if (grid_visited[cc.l][cc.c]) {
				continue;
			}

			struct noeud_t *next = &Q[n];
			n = (n + 1) % (lignes * colonnes);
			next->c = cc;
			if (noeud.dir == -1) {
				next->dir = d;
				r = d;
			} else {
				next->dir = noeud.dir;
			}
			grid_visited[cc.l][cc.c] = 1;
		}
	}

	free(Q);
	for (l = 0; l < lignes; l++) {
		free(grid_visited[l]);
	}
	free(grid_visited);
	return r;
}

void decision_ia()
{
	//int n_dir = (double)rand() / (double)RAND_MAX * 4;

//      if ((dir_ia + n_dir) % 2) {
	int i =
	    (snake_ia.indice_deb + snake_ia.longueur - 1) % (lignes * colonnes);
	dir_ia = pcc(&snake_ia.coords[i], &bonus);
//      }
}

int avance_ia()
{
	static int allonge = 0;
	if (snake_ia.longueur < 4 || allonge) {
		snake_ia.longueur++;
		allonge = 0;
	} else {
		print_locate(snake_ia.coords[snake_ia.indice_deb].l,
			     snake_ia.coords[snake_ia.indice_deb].c, ' ');
		grid[snake_ia.coords[snake_ia.indice_deb].l][snake_ia.
							     coords[snake_ia.
								    indice_deb].
							     c] = 0;
		snake_ia.indice_deb =
		    (snake_ia.indice_deb + 1) % (lignes * colonnes);
	}

	int i =
	    (snake_ia.indice_deb + snake_ia.longueur - 1) % (lignes * colonnes);
	int i_ =
	    (snake_ia.indice_deb + snake_ia.longueur - 2) % (lignes * colonnes);
	snake_ia.coords[i].l = snake_ia.coords[i_].l + directions[dir_ia].l;
	snake_ia.coords[i].c = snake_ia.coords[i_].c + directions[dir_ia].c;

	if (snake_ia.coords[i].l == bonus.l && snake_ia.coords[i].c == bonus.c) {
		genere_bonus();
		allonge = 1;
	}

	if (!dans_aire_jeu(snake_ia.coords[i].l, snake_ia.coords[i].c)
	    || collision(snake_ia.coords[i].l, snake_ia.coords[i].c)) {
		return -1;
	}

	print_locate(snake_ia.coords[i].l, snake_ia.coords[i].c, '2');
	grid[snake_ia.coords[i].l][snake_ia.coords[i].c] = 1;
	return 0;
}

static struct termios oldt;

static void handler(int signum __attribute__ ((unused)))
{
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	exit(0);
}

int game()
{
	int l, c;
	struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	lignes = ws.ws_row - 1;
	colonnes = ws.ws_col;
	snake.coords = malloc(lignes * colonnes * sizeof(struct coord_t));
	snake_ia.coords = malloc(lignes * colonnes * sizeof(struct coord_t));
	grid = malloc(sizeof(int *) * lignes);
	for (l = 0; l < lignes; l++) {
		grid[l] = malloc(sizeof(int) * colonnes);
	}

	clear_screen();
	printf("\033[0;0H");

	for (l = 0; l < lignes; l++) {
		printf("\033[30m\033[40m#");
		grid[l][0] = 1;
		for (c = 1; c < colonnes - 1; c++) {
			if (l == 0 || l == lignes - 1) {
				printf("\033[30m\033[40m#");
				grid[l][c] = 1;
			} else {
				printf("\033[37m\033[107m ");
				grid[l][c] = 0;
			}
		}
		printf("\033[30m\033[40m#");
		grid[l][colonnes - 1] = 1;
	}
	fflush(stdout);

	setvbuf(stdin, NULL, _IO_MAGIC | _IONBF, 0);

	struct termios newt;
	tcgetattr(STDIN_FILENO, &oldt);

	signal(SIGINT, handler);

	memcpy(&newt, &oldt, sizeof(newt));
	
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	fcntl(STDIN_FILENO, F_SETFL, (void *)O_NONBLOCK);

	init_snake();
	int p = 0, ia;
	do {
		// IA play first.
		decision_ia();
		ia = avance_ia();

		if (ia == -1) {
			continue;
		}

		usleep(100000);

		// Player second.
		thread_input();
		p = avance_snake();
	} while (ia != -1 && p != -1);

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

	return p;
}

int main()
{
	srand(time(NULL));

	int r = game();

	if (r == -1) {
		printf("\033[0m Perdu !\n");
	} else {
		printf("\033[0m Bravo !\n");
	}

	return 0;
}
