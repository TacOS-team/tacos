/**
 * @file snake.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
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
 
/* Hall of Fame 
 * ------------
 * 1) Nico: 49pts
 * 2) Max:  35pts
 * 3)
 * 4)
 * 5)
 */
 

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define LIGNES 24
#define COLONNES 79

static int dir = 2, dir_ = 2;
static int score = 0;

struct coord_t {
	int l, c;
};

struct snake_t {
	int longueur;
	int indice_deb;
	struct coord_t coords[LIGNES * COLONNES];
};

static struct snake_t snake;
static struct coord_t directions[4] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};

static struct coord_t bonus;

static void haut() {
	if (dir_ != 1) {
		dir = 0;
	}
}

static void bas() {
	if (dir_ != 0) {
		dir = 1;
	}
}

static void droite() {
	if (dir_ != 3) {
		dir = 2;
	}
}

static void gauche() {
	if (dir_ != 2) {
		dir = 3;
	}
}

void thread_input() {
	char c;
	c = getchar();
	switch(c) {
		case 'z': haut(); break;
		case 's': bas(); break;
		case 'd': droite(); break;
		case 'q': gauche(); break;
	}
}

void clear_screen() {
	printf("\033[2J");
	fflush(stdout);
}

void print_locate(int l, int c, char car) {
	printf("\033[%d;%df%c\033[%d;%df", l, c, car, LIGNES, COLONNES);
	fflush(stdout);
}

static int collision(int l, int c, int n){
	int i;
	for (i = 0; i < snake.longueur - n; i++) {
		if (l == snake.coords[(snake.indice_deb + i) % (LIGNES * COLONNES)].l && c == snake.coords[(snake.indice_deb + i) % (LIGNES * COLONNES)].c) {
			return 1;
		}
	}
	return 0;
}

static void genere_bonus() {
	do {
		bonus.l = (rand() % (LIGNES-2)) + 2;
		bonus.c = (rand() % (COLONNES-2)) + 2;
	} while (collision(bonus.l, bonus.c, 0));
	print_locate(bonus.l, bonus.c, '+');
}

void init_snake() {
	dir = 2;
	dir_ = 2;
	snake.longueur = 1;
	snake.indice_deb = 0;
	snake.coords[snake.indice_deb].c = COLONNES/3;
	snake.coords[snake.indice_deb].l = LIGNES/2;
	print_locate(snake.coords[snake.indice_deb].l, snake.coords[snake.indice_deb].c, '*');

	genere_bonus();
}

static inline int dans_aire_jeu(int l, int c) {
	return l > 1 && l < LIGNES && c > 1 && c < COLONNES;
}

int avance_snake() {
	static int allonge = 0;
	if (snake.longueur < 4 || allonge) {
		snake.longueur++;
		allonge = 0;
	} else {
		print_locate(snake.coords[snake.indice_deb].l, snake.coords[snake.indice_deb].c, ' ');
		snake.indice_deb = (snake.indice_deb + 1) % (LIGNES * COLONNES);
	}

	int i = (snake.indice_deb + snake.longueur - 1) % (LIGNES * COLONNES);
	int i_ = (snake.indice_deb + snake.longueur - 2) % (LIGNES * COLONNES);
	snake.coords[i].l = snake.coords[i_].l + directions[dir].l;
	snake.coords[i].c = snake.coords[i_].c + directions[dir].c;

	dir_ = dir;

	if (snake.coords[i].l == bonus.l && snake.coords[i].c == bonus.c) {
		score += 1;
		genere_bonus();
		allonge = 1;
	}

	if (!dans_aire_jeu(snake.coords[i].l, snake.coords[i].c) || collision(snake.coords[i].l, snake.coords[i].c, 1)) {
			return -1;
	}

	print_locate(snake.coords[i].l, snake.coords[i].c, '*');
	return 0;
}

static struct termios oldt;

static void handler(int signum __attribute__((unused))) {
	tcsetattr( STDIN_FILENO, TCSETS, &oldt );
	exit(0);
}

void game() {
	int l, c;

	clear_screen();
	printf("\033[0;0H");
	for(l = 0; l < LIGNES; l++) {
		printf("#");
		for(c = 1; c < COLONNES-1; c++) {
			if (l == 0 || l == LIGNES - 1) {
				printf("#");
			} else {
				printf(" ");
			}
		}
		printf("#\n");
	}

  setvbuf(stdin, NULL, _IO_MAGIC | _IONBF, 0);

	struct termios newt;
	tcgetattr( STDIN_FILENO, &oldt );

	signal(SIGINT, handler);

	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSETS, &newt );
	fcntl(STDIN_FILENO, F_SETFL, (void*)O_NONBLOCK);

  init_snake();

	while(avance_snake() != -1) {
		usleep(100000);
		thread_input();
	}

	tcsetattr( STDIN_FILENO, TCSETS, &oldt );

}

int main() {
	srand(time(NULL));
 
	game();

	printf("\033[0m\nScore : %d\n", score);

	return 0;
}
