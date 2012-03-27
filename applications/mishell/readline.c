#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>


#define KUP 454081
#define KDOWN 454082
#define KRIGHT 454083
#define KLEFT 454084
#define KPGUP 58120958
#define KPGDOWN 58121086
#define KDEL 58120702

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

	//XXX: Cette ligne plante sous bochs... remplacée par un memcpy...
	//struct termios newt = oldt;
	struct termios newt;
	memcpy(&newt, &oldt, sizeof(newt));
	
	cfmakeraw(&newt);
	tcsetattr(0, TCSANOW, &newt);

	char *buf = malloc(sizeof(char) * 1024);
	int count = 0;
	int pos = 0;
	buf[count] = '\0';

	do {
		int t, m;
		int c = getbigchar();
		switch (c) {
		case 3:
			buf[0] = '\0';
		case 10:
		case 13:
			end = 1;
			printf("\n");
		case KLEFT:
			if (pos > 0) {
				pos--;
				if (cur_col > 0) {
					printf("\033[D");
					cur_col--;
				} else {
					printf("\033[A\033[%dC", colonnes - 1);
					cur_col = colonnes - 1;
				}
			}
			break;
		case KRIGHT:
			if (pos < count) {
				pos++;
				if (cur_col < colonnes - 1) {
					printf("\033[C");
					cur_col++;
				} else {
					printf("\033[B\033[%dD", colonnes - 1);
					cur_col = 0;
				}
			}
			break;
		case KDEL:
			if (pos == count) {
				break;
			}
			for (i = pos; i < count; i++) {
				buf[i] = buf[i + 1];
			}
			count--;

			printf("%s ", &buf[pos]);
			
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
		case 127:
			if (pos == 0) {
				break;
			}
			if (pos < count) {
				for (i = pos - 1; i < count; i++) {
					buf[i] = buf[i + 1];
				}
			} else {
				buf[pos - 1] = '\0';
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
			printf("%s ", &buf[pos]);
			
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
			if (c >= 32 && c < 128) {
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

	//tcsetattr(0, TCSANOW, &oldt);

	return buf;
}
