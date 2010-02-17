#include <types.h>
#include <ioports.h>

/* Some screen stuff. */
/* The number of columns. */
#define COLUMNS                 80
/* The number of lines. */
#define LINES                   25
/* The attribute of an character. */
#define ATTRIBUTE               0x02
/* The video memory address. */
#define VIDEO                   0xB8000

#define CRT_REG_INDEX 0x3d4
#define CRT_REG_DATA  0x3d5
#define CURSOR_POS_MSB 0x0E
#define CURSOR_POS_LSB 0x0F


/* Variables. */
/* Save the X position. */
static int xpos;
/* Save the Y position. */
static int ypos;
/* Point to the video memory. */

typedef struct {
	unsigned char character;
	unsigned char attribute;
} __attribute__ ((packed)) x86_video_mem[LINES*COLUMNS];

/** The base pointer for the video memory */
/* volatile pour éviter des problèmes d'optimisation de gcc. */
static volatile x86_video_mem *video = (volatile x86_video_mem*)VIDEO;

static void scrollup();
static void updateCursorPosition();

void disableCursor()
{
  /* CRT index port => ask for access to register 0xa ("cursor
	   start") */
	outb(0x0a, CRT_REG_INDEX);

	/* (RBIL Tables 708 & 654) CRT Register 0xa => bit 5 = cursor OFF */
	outb(1 << 5, CRT_REG_DATA);
}

/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
void cls (void) {
	int i;
 
	for (i = 0; i < COLUMNS * LINES; i++) {
	  (*video)[i].character = 0;
	  (*video)[i].attribute = ATTRIBUTE;
	}

	xpos = 0;
	ypos = 0;
}

/* Convert the integer D to a string and save the string in BUF. If
	 BASE is equal to 'd', interpret that D is decimal, and if BASE is
	 equal to 'x', interpret that D is hexadecimal. */
void itoa (char *buf, int base, int d) {
	char *p = buf;
	char *p1, *p2;
	unsigned long ud = d;
	int divisor = 10;

	/* If %d is specified and D is minus, put `-' in the head. */
	if (base == 'd' && d < 0) {
		*p++ = '-';
		buf++;
		ud = -d;
	} else if (base == 'x') {
		divisor = 16;
	}

	/* Divide UD by DIVISOR until UD == 0. */
	do {
		int remainder = ud % divisor;

		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	} while (ud /= divisor);

	/* Terminate BUF. */
	*p = 0;

	/* Reverse BUF. */
	p1 = buf;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

static void scrollup() {
	int c, l;

	/*
	 * On décalle vers le haut.
	 */
	for (l = 0; l < LINES - 1; l++) {
		for (c = 0; c < COLUMNS; c++) {
			(*video)[l*COLUMNS + c].character = (*video)[(l+1)*COLUMNS + c].character;
			(*video)[l*COLUMNS + c].attribute = (*video)[(l+1)*COLUMNS + c].attribute;
		}
	}

	/*
	 * On met des espaces sur la dernière ligne
	 */
	for (c = 0; c < COLUMNS; c++) {
		(*video)[(LINES-1) * COLUMNS + c].character = ' ';
		(*video)[(LINES-1) * COLUMNS + c].attribute = ATTRIBUTE;
	}
}

static void updateCursorPosition()
{
  int pos = xpos + ypos*COLUMNS;

  outb(CURSOR_POS_LSB, CRT_REG_INDEX);
  outb((uint8_t) pos, CRT_REG_DATA);
  outb(CURSOR_POS_MSB, CRT_REG_INDEX);
  outb((uint8_t) (pos >> 8), CRT_REG_DATA);
}

void newline()
{
  xpos = 0;
	ypos++;
	if (ypos >= LINES)
  {
		scrollup();
		ypos = LINES - 1;
	}
}

/* Put the character C on the screen. */
void putchar (int c) {
	if (c == '\n' || c == '\r')
  {
    newline();
	} else
  {
  	(*video)[xpos + ypos * COLUMNS].character = c & 0xFF;
  	(*video)[xpos + ypos * COLUMNS].attribute = ATTRIBUTE;

  	xpos++;
	  if (xpos >= COLUMNS)
      newline();
  }

  updateCursorPosition();
}

/* Format a string and print it on the screen, just like the libc
	 function printf. */
void printf (const char *format, ...) {
	char **arg = (char **) &format;
	int c;
	char buf[20];

	arg++;

	while ((c = *format++) != 0) {
		if (c != '%') {
			putchar (c);
		} else {
			char *p;

			c = *format++;
			switch (c) {
				case 'd':
				case 'u':
				case 'x':
					itoa (buf, c, *((int *) arg++));
					p = buf;
					goto string;
					break;
				case 's':
					p = *arg++;
					if (! p)
						p = "(null)";
				string:
					while (*p)
						putchar (*p++);
					break;
				default:
					putchar (*((int *) arg++));
					break;
			}
		}
	}
}
