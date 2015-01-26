#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include "history.h"


#define KUP 454081
#define KDOWN 454082
#define KRIGHT 454083
#define KLEFT 454084
#define KPGUP 58120958
#define KPGDOWN 58121086
#define KDEL 58120702
#define KHOME 58120446
#define KEND 58120830

static int getbigchar()
{
	int r = getchar();

	int flags;
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

void backspace(char* buf, int *pos, int *cur_col, int colonnes, int *count) {
	int i;
	if (*pos == 0) {
		return;
	}
	if (*pos < *count) {
		for (i = *pos - 1; i < *count; i++) {
			buf[i] = buf[i + 1];
		}
	} else {
		buf[*pos - 1] = '\0';
	}
	if (*cur_col > 0) {
		printf("\033[D");
		(*cur_col)--;
	} else {
		printf("\033[A\033[%dC", colonnes - 1);
		*cur_col = colonnes - 1;
	}
	(*count)--;
	(*pos)--;
	printf("%s ", &buf[*pos]);
	
	int t = *count - *pos + 1;
	int m = (t + *cur_col - 1) / colonnes;
	if (m > 0) {
		printf("\033[%dA", m);
	}

	t -= m * colonnes;
	if (t > 0) {
		printf("\033[%dD", t);
	} else if (t < 0) {
		printf("\033[%dC", -t);
	}
}

void delete(char* buf, int pos, int cur_col, int colonnes, int *count) {
	int i;
	if (pos == *count) {
		return;
	}
	for (i = pos; i < *count; i++) {
		buf[i] = buf[i + 1];
	}
	(*count)--;

	printf("%s ", &buf[pos]);
	
	int t = *count - pos + 1;
	int m = (t + cur_col - 1) / colonnes;
	if (m > 0) {
		printf("\033[%dA", m);
	}

	t -= m * colonnes;
	if (t > 0) {
		printf("\033[%dD", t);
	} else if (t < 0) {
		printf("\033[%dC", -t);
	}
}

char *readline(const char *prompt)
{
	int i;
	int end = 0;
	int cur_col = 0;
	if (prompt) {
		printf("%s", prompt);
		cur_col = strlen(prompt);
		fflush(stdout);
	}
	struct winsize ws;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	int colonnes = ws.ws_col;

	struct termios oldt;
	tcgetattr(0, &oldt);

	struct termios newt = oldt;
	
	cfmakeraw(&newt);
	tcsetattr(0, TCSANOW, &newt);

	char *buf = malloc(sizeof(char) * 1024);
	int count = 0;
	int pos = 0;
	buf[count] = '\0';

	using_history();

	do {
		int t, m;
		int c = getbigchar();
		switch (c) {
		case -1:
			if (count == 0) {
				free(buf);
				buf = NULL;
			} else {
				printf("\n");
			}
			end = 1;
			break;
		case KHOME:
		case 1: /*C^a*/
			while (pos > 0) {
				move_left(&pos,&cur_col,colonnes);
			}
			break;
		case 3:
			buf[0] = '\0';
		case KEND:
		case 5: /*C^e*/
			while (pos != count) {
				move_right(&pos,&cur_col,colonnes,count);
			}
			break;
		case 13:
			end = 1;
			printf("\n");
		case 2: /*C^b*/
		case KLEFT:
			move_left(&pos, &cur_col, colonnes);
			break;
		case 6:/*C^f*/
		case KRIGHT:
			move_right(&pos, &cur_col, colonnes, count);
			break;
		case 21: /*C^u*/
			while (pos > 0) {
				backspace(buf, &pos, &cur_col, colonnes, &count);
			}
			while (count > 0) {
				delete(buf, pos, cur_col, colonnes, &count);
			}
			break;
		case KUP:
			{
				HIST_ENTRY* up = previous_history();
				if (up) {
					while (pos > 0) {
						backspace(buf, &pos, &cur_col, colonnes, &count);
					}
					while (count > 0) {
						delete(buf, pos, cur_col, colonnes, &count);
					}
					strcpy(buf, up->cmd);

					printf("%s", buf);
					count = pos = strlen(buf);
					cur_col = (cur_col + count) % colonnes;
				}
			}
			break;
		case KDOWN:
			{
				HIST_ENTRY* next = next_history();
				while (pos > 0) {
					backspace(buf, &pos, &cur_col, colonnes, &count);
				}
				while (count > 0) {
					delete(buf, pos, cur_col, colonnes, &count);
				}
				if (next) {
					strcpy(buf, next->cmd);

					printf("%s", buf);
					count = pos = strlen(buf);
					cur_col = (cur_col + count) % colonnes;
				} else {
					buf[0] = '\0';
					count = pos = 0;
				}
			}
		case KDEL:
			delete(buf, pos, cur_col, colonnes, &count);
			break;
		case 127:
			backspace(buf, &pos, &cur_col, colonnes, &count);
			break;
		default:
			if (c > 26 && c < 256) {
				// Insert mode.
				for (i = count; i >= pos; i--) {
					buf[i + 1] = buf[i];
				}
				count++;

				buf[pos++] = c;
				if (pos < count) {
					printf("%s", buf + pos - 1);

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

					if (cur_col < colonnes - 1) {
						printf("\033[C");
						cur_col++;
					} else {
						printf("\033[B\033[%dD", colonnes - 1);
						cur_col = 0;
					}
				} else {
					printf("%c", c);

					if (cur_col < colonnes - 1) {
						cur_col++;
					} else {
						cur_col = 0;
						printf(" \033[D");
					}
				}
			}
		}
		fflush(stdout);
	} while (!end);

	tcsetattr(0, TCSANOW, &oldt);

	if (buf) {
		add_history(buf);
	}

	return buf;
}
