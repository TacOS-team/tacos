#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <fcntl.h>

#define KUP 454081
#define KDOWN 454082
#define KRIGHT 454083
#define KLEFT 454084
#define KPGUP 58120958
#define KPGDOWN 58121086
#define KDEL 58120702

static int getbigchar() {
    int r = getchar();

    int flags;
    if ((flags = fcntl(0, F_GETFL, (void*)0)) == -1) {
        flags = 0;
    }
    fcntl(0, F_SETFL, (void*)(flags | O_NONBLOCK));
    int c;
    while ((c = getchar()) != -1) r = r * 128 + c;
    fcntl(0, F_SETFL, (void*)flags);

    return r;
}

char* readline(const char *prompt) {
	int i;
	int end = 0;
	if (prompt) {
		printf("%s", prompt);
		fflush(stdout);
	}

	struct termios oldt;
	tcgetattr(0, &oldt );
	struct termios newt = oldt;
	cfmakeraw(&newt);
	tcsetattr(0, TCSANOW, &newt);

	char* buf = malloc(sizeof(char) * 1024);
	int count = 0;
	int pos = 0;
	buf[count] = '\0';

	do {
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
					printf("\033[D");
				}
				break;
			case KRIGHT:
				if (pos < count) {
					pos++;
					printf("\033[C");
				}
				break;
      case KDEL:
        if (pos == count) {
          break;
        }
        for (i = pos; i < count; i++) {
          buf[i] = buf[i+1];
        }
        count--;
        printf("%s \033[%dD", &buf[pos], count - pos + 1);
        break;
			case 127:
				if (pos == 0) {
					break;
				}
				if (pos < count) {
					for (i = pos-1; i < count; i++) {
						buf[i] = buf[i+1];
					}
					printf("\033[D");
				} else {
					buf[pos-1] = '\0';
					printf("\033[D \033[D");
				}
				count--;
				pos--;
				printf("%s \033[%dD", &buf[pos], count - pos + 1);
				break;
			default:
				if (c >= 32 && c < 128) {
					// Insert mode.
					for (i = count; i >= pos; i--) {
						buf[i+1] = buf[i];
					}
					count++;

					buf[pos++] = c;
					if (count > pos) {
						printf("%s\033[%dD", buf + pos - 1, count - pos);
					} else {
						printf("%c", c);
					}
				}
		}
		fflush(stdout);
	} while (!end);


	tcsetattr(0, TCSANOW, &oldt);

	return buf;
}
