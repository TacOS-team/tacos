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
 * Description de ce que fait le fichier
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <vga_types.h>

#define HAUTEUR 200
#define LARGEUR 320

#define FACTEUR 4

#define LIGNES 40
#define COLONNES 40

static int dir = 2, dir_ = 2;
static int score = 0;

static char buffer[LARGEUR*HAUTEUR];

void _put_pixel(char c, int x, int y, char* buffer) {
	if (x >= 0 && x < LARGEUR && y >= 0 && y < HAUTEUR) {
		buffer[x+y*LARGEUR] = c;
	}
}

void put_pixel(int x, int y, int color) {
	int i,j;
	for(i=0; i<FACTEUR; i++) {
		for(j=0; j<FACTEUR; j++) {
			_put_pixel(color,x*FACTEUR+i,y*FACTEUR+j,buffer);
		}
	}	
}

struct coord_t {
	int l, c;
};

struct snake_t {
	int longueur;
	int indice_deb;
	struct coord_t coords[LIGNES * COLONNES];
};

static struct snake_t snake;
static struct coord_t directions[4] = {{0, -1},{0, 1},{1, 0}, {-1, 0}};

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
	put_pixel(bonus.l, bonus.c,6);
}

void init_snake() {
	dir = 2;
	dir_ = 2;
	snake.longueur = 1;
	snake.indice_deb = 0;
	snake.coords[snake.indice_deb].c = COLONNES/3;
	snake.coords[snake.indice_deb].l = LIGNES/2;
	put_pixel(snake.coords[snake.indice_deb].l, snake.coords[snake.indice_deb].c,1);

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
		put_pixel(snake.coords[snake.indice_deb].l, snake.coords[snake.indice_deb].c,0);
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

	put_pixel(snake.coords[i].l, snake.coords[i].c,1);
	return 0;
}

void game() {
  setvbuf(stdin, NULL, _IO_MAGIC | _IONBF, 0);

	struct termios oldt, newt;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSETS, &newt );
	fcntl(STDIN_FILENO, F_SETFL, (void*)O_NONBLOCK);
	syscall(SYS_VGASETMODE, vga_mode_320x200x256, 0, 0);
  init_snake();

	while(avance_snake() != -1) {
		usleep(100000);
		thread_input();
		syscall(SYS_VGAWRITEBUF, (uint32_t)buffer, 0, 0);
	}

	tcsetattr( STDIN_FILENO, TCSETS, &oldt );

}

int main() {

	
	srand(time(NULL));
 
	game();

	printf("\033[0m\nScore : %d\n", score);

	return 0;
}
