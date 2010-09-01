#define __KTW_COLUMNS 80
#define __KTW_LINES 25

#include <video.h>
#include <fcntl.h>
#include <kfcntl.h>
#include <stdlib.h>
#include <console.h>

void kprintf(const char *format, ...) {
	static int n = -1;
	if (n == -1) {
		n = get_available_console(NULL);
	}

	char **arg = (char **) &format;
	int c;
	char buf[20];

	arg++;

	while ((c = *format++) != 0) {
		if (c != '%')
			kputchar(n, c);
		else {
			char *p;

			c = *format++;
			switch (c) {
			case 'd':
			case 'u':
			case 'x':
				itoa(buf, c, *((int *) arg++));
				p = buf;
				goto string;
				break;

			case 's':
				p = *arg++;
				if (!p)
					p = "(null)";

				string: while (*p)
					kputchar(n, *p++);
				break;

			default:
				kputchar(n, *((int *) arg++));
				break;
			}
		}
	}
}
