#include <types.h>
#include <ioports.h>

/* Some screen stuff. */
/* The number of columns. */
#define COLUMNS                 80
/* The number of lines. */
#define LINES                   25
/* The video memory address. */
#define VIDEO                   0xB8000

#define CRT_REG_INDEX 0x3d4
#define CRT_REG_DATA  0x3d5
#define CURSOR_POS_MSB 0x0E
#define CURSOR_POS_LSB 0x0F

#define DEFAULT_ATTRIBUTE_VALUE 0x09

/* Variables. */
static int finnouMode = 0; //disable par défaut, quand même 
/* Attribute of a character */
static uint8_t attribute = DEFAULT_ATTRIBUTE_VALUE;
/* Point to the video memory. */

typedef struct {
	unsigned char character;
	unsigned char attribute;
} __attribute__ ((packed)) x86_video_mem[LINES*COLUMNS];

typedef struct {
	x86_video_mem buffer;
	int bottom_buffer;
	/* Save the X position. */
	int xpos;
	/* Save the Y position. */
	int ypos;
	bool disable_cursor;
} buffer_video_t;

/** The base pointer for the video memory */
/* volatile pour éviter des problèmes d'optimisation de gcc. */
static volatile x86_video_mem *video = (volatile x86_video_mem*)VIDEO;
static buffer_video_t buffer_standard;
static buffer_video_t buffer_debug;
static buffer_video_t *buffer_video = &buffer_standard;

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

void refresh (void) {
	int i;
	for (i = 0; i < COLUMNS * LINES; i++) {
	  (*video)[i].character = buffer_video->buffer[(i + buffer_video->bottom_buffer*80)%(25*80)].character;
	  (*video)[i].attribute = buffer_video->buffer[(i + buffer_video->bottom_buffer*80)%(25*80)].attribute;
	}
}

/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
void cls (void) {
	int i;
 
	for (i = 0; i < COLUMNS * LINES; i++) {
	  buffer_video->buffer[i].character = 0;
	  buffer_video->buffer[i].attribute = attribute;
	}

	refresh();

	buffer_video->xpos = 0;
	buffer_video->ypos = 0;
}

void switchDebugBuffer() {
	buffer_video = &buffer_debug;
	refresh();
	if (buffer_video->disable_cursor) {
		disableCursor();
	} else {
		updateCursorPosition();
	}
}

void switchStandardBuffer() {
	buffer_video = &buffer_standard;
	refresh();
	if (buffer_video->disable_cursor) {
		disableCursor();
	} else {
		updateCursorPosition();
	}
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

int atoi(const char* __nptr)
{
	int ret = 0;
	int sig = 1;
	
	if(*__nptr == '-')
	{
		__nptr++;
		sig = -1;
	}
	else if(*__nptr == '+')
		__nptr++;
	
	while(*__nptr)
	{
		if(*__nptr >= '0' && *__nptr<='9')
			ret = 10*ret+(*__nptr-'0');
		else
			return -1;
		
		__nptr++;
	}
	
	return ret;
}

static void scrollup() {
	int c;

	buffer_video->bottom_buffer++;

	/*
	 * On met des espaces sur la dernière ligne
	 */
	for (c = 0; c < COLUMNS; c++) {
		buffer_video->buffer[((buffer_video->bottom_buffer+LINES-1) * COLUMNS + c)%(25*80)].character = ' ';
		buffer_video->buffer[((buffer_video->bottom_buffer+LINES-1) * COLUMNS + c)%(25*80)].attribute = attribute;
	}

	refresh();
}

static void updateCursorPosition()
{
  int pos = buffer_video->xpos + buffer_video->ypos*COLUMNS;

  outb(CURSOR_POS_LSB, CRT_REG_INDEX);
  outb((uint8_t) pos, CRT_REG_DATA);
  outb(CURSOR_POS_MSB, CRT_REG_INDEX);
  outb((uint8_t) (pos >> 8), CRT_REG_DATA);
}

void newline()
{
	buffer_video->xpos = 0;
	buffer_video->ypos++;
	if (buffer_video->ypos >= LINES) {
		scrollup();
		buffer_video->ypos = LINES - 1;
	}
}

void putchar_position(char c, int x, int y) {
	buffer_video->buffer[x + ((y+buffer_video->bottom_buffer)%25) * COLUMNS].character = c & 0xFF;
	buffer_video->buffer[x + ((y+buffer_video->bottom_buffer)%25) * COLUMNS].attribute = attribute;
	(*video)[x + y * COLUMNS].character = c & 0xFF;
	(*video)[x + y * COLUMNS].attribute = attribute;
}

/* Put the character C on the screen. */
void putchar (char c) {
	if (c == '\n' || c == '\r') {
		newline();
	} else {
		putchar_position(c, buffer_video->xpos, buffer_video->ypos);
	  	buffer_video->xpos++;
		if (buffer_video->xpos >= COLUMNS)
			newline();
	}
  
	if(finnouMode)
		attribute = 0x0F & (attribute + 13 % 8);

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

void sprintf (char* string, const char *format, ...) {
	char **arg = (char **) &format;
	int c;
	int size;
	char buf[20];
	int i=0;
	
	arg++;

	while ((c = *format++) != 0) {
		if (c != '%') {
			string[i] = c;
			i++;
		} else {
			char *p;
			size = -1;
			c = *format++;
			
			if(c == '.')
			{
				char c2;
				size = 0;
				
				for(c2 = *format++; c2>='0' && c2<='9' ;c2 = *format++)
					buf[size++] = c2;
				
				buf[size] = '\0';
				size = atoi(buf);
				
				c = *(format-1);
			}	
			
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
					while (*p && size--)
					{
						string[i++] = *p++;
					}
					break;
				default:
					string[i++] = (*((int *) arg++));
					break;
			}
		}
	}
}

void enableFinnouMode(int enable)
{
  finnouMode = enable;
}

void set_attribute(uint8_t background, uint8_t foreground)
{
  attribute = ((background & 0xF) << 4) | (foreground & 0xF); 
}

void reset_attribute()
{
  attribute = DEFAULT_ATTRIBUTE_VALUE;
}

