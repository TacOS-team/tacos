#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "init.h"

void open_bulot(int i, int colonnes, int fall)
{
	FILE *bulot;
	bulot = fopen("../files/bulot.ansi", "r");
	int c, j, lol;
	printf("\033[2J");
	printf("\033[;H");
	printf("\033[%dC", colonnes - i);

	if (fall == 1) {
		for (j = 0; j < colonnes - i; ++j) {
			printf("\033[%dC", j);
			if (j % 2) {
				printf("o                 O\n");
			} else {
				printf("  O              o\n");
			}
		}

	}
	while ((c = fgetc(bulot)) != EOF) {
		if (fall == 2 || fall == 1) {
			lol = rand();
			printf("\033[%dm", 30 + (lol % 8));
		}
		printf("%c", c);
		if (c == '\n') {
			printf("\033[%dC", colonnes - i);
		}
	}
	printf("\033[%dm", 39);
	printf("\n");
	fclose(bulot);
}

void print_file_90(char *file)
{
	FILE *f;
	f = fopen(file, "r");
	char buf[1024];
	int i;

	printf("\033[2J");
	printf("\033[;H");
	printf("\033[s");

	while (fgets(buf, 1024, f) != NULL) {
		printf("\033[u");
		printf("\033[1C");
		printf("\033[s");
		for (i = 0; buf[i] != '\0'; ++i) {
			printf("%c", buf[i]);
			printf("\033[1D\033[1B");
		}
	}
	printf("\n");
	fclose(f);
}

void cours_bulot_cours (int i, int colonnes, int fall)
{
    fall++;
	printf("\033[2J");
	printf("\033[;H");
	printf("\033[%dC", colonnes - i);
	if (i % 2) {
		printf("<@.\n");
	} else {
		printf("<@_.\n");
	}
}

void sl(void (*ptfct)(int, int, int), int fall)
{
    struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	int colonnes = ws.ws_col;

	for (; colonnes > 50; --colonnes) {
		ptfct(colonnes, ws.ws_col, fall);
		usleep(100000 / 2);
	}
}
