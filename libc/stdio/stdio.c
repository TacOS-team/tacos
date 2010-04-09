#include <types.h>
#include <ioports.h>
#include <ctype.h>

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
	uint8_t attribute;
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
 
	buffer_video->attribute = DEFAULT_ATTRIBUTE_VALUE;

	for (i = 0; i < COLUMNS * LINES; i++) {
	  buffer_video->buffer[i].character = 0;
	  buffer_video->buffer[i].attribute = buffer_video->attribute;
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
		buffer_video->buffer[((buffer_video->bottom_buffer+LINES-1) * COLUMNS + c)%(25*80)].attribute = buffer_video->attribute;
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
	updateCursorPosition();
}

void kputchar_position(char c, int x, int y) {
	buffer_video->buffer[x + ((y+buffer_video->bottom_buffer)%LINES) * COLUMNS].character = c & 0xFF;
	buffer_video->buffer[x + ((y+buffer_video->bottom_buffer)%LINES) * COLUMNS].attribute = buffer_video->attribute;
	(*video)[x + y * COLUMNS].character = c & 0xFF;
	(*video)[x + y * COLUMNS].attribute = buffer_video->attribute;
}

void cursor_up() {
	if (buffer_video->ypos > 0)
		buffer_video->ypos--;
	updateCursorPosition();
}

void cursor_down() {
	if (buffer_video->ypos < LINES - 1)
		buffer_video->ypos++;
	updateCursorPosition();
}

void cursor_back() {
	if (buffer_video->xpos > 0)
		buffer_video->ypos--;
	updateCursorPosition();
}

void cursor_forward() {
	if (buffer_video->xpos < COLUMNS -1)
		buffer_video->xpos++;
	updateCursorPosition();
}

void lineup() {
	cursor_up();
	buffer_video->xpos = 0;
	updateCursorPosition();
}

void cursor_move_col(int n) {
	buffer_video->xpos = n-1;
	updateCursorPosition();
}

void cursor_move(int n, int m) {
	if (n > 0 && n <= LINES) {
		buffer_video->ypos = n-1;
	}
	if (m > 0 && m <= COLUMNS) {
		buffer_video->xpos = m-1;
	}
}

void backspace() {
	if (buffer_video->xpos > 0) {
		buffer_video->xpos--;
	} else if (buffer_video->ypos > 0) {
		buffer_video->ypos--;
		buffer_video->xpos = COLUMNS - 1;
	}

	kputchar_position(' ', buffer_video->xpos, buffer_video->ypos); 
}

/* Put the character C on the screen. */
void kputchar (char c) {
	static bool escape_char = FALSE;
	static bool ansi_escape_code = FALSE;
	static bool ansi_second_val = FALSE;
	static int val = 0, val2 = 0;

	if (escape_char) {
		if (ansi_escape_code) {
			if (isdigit(c)) {
				if (ansi_second_val) {
					val2 = val2 * 10 + c - '0';
				} else {
					val = val * 10 + c - '0';
				}
			} else {
				escape_char = FALSE;
				ansi_second_val = FALSE;
				ansi_escape_code = FALSE;
				if (val == 0 && c != 'J') val = 1;
				if (val2 == 0) val2 = 1;
				switch(c) {
					case 'A':
						while (val--) cursor_up();
						break;
					case 'B':
						while (val--) cursor_down();
						break;
					case 'C':
						while (val--) cursor_forward();
						break;
					case 'D': 
						while (val--) cursor_back();
						break;
					case 'E':
						while (val--) newline();
						break;
					case 'F':
						while (val--) lineup();
						break;
					case 'G':
						cursor_move_col(val);
						break;
					case 'f':
					case 'H':
						cursor_move(val, val2);
						break;
					case ';':
						escape_char = TRUE;
						ansi_second_val = TRUE;
						ansi_escape_code = TRUE;
						val2 = 0;
						break;
					case 'm':
						if (val == 0) {
							reset_attribute();
						} else if (val >= 30 && val <= 37) {
							set_foreground(val - 30); // FIXME : faire un switch pour gérer les 8 couleurs possibles.
						} else if (val >= 40 && val <= 47) {
							set_background(val - 40);
						}
						break;
					case 'J':
						if (val == 0) {
							
						} else if (val == 1) {

						} else if (val == 2) {
							cls();
						}
						break;
				}
			}
		} else {
			if (c == '[') {
				ansi_escape_code = TRUE;
				val = 0;
			} else {
				escape_char = FALSE;
			}
		}
	} else if (c == '\033') {
		escape_char = TRUE;
	} else if (c == '\n' || c == '\r') {
		newline();
	} else if (c == '\b') {
		backspace();
	} else {
		kputchar_position(c, buffer_video->xpos, buffer_video->ypos);
	  	buffer_video->xpos++;
		if (buffer_video->xpos >= COLUMNS)
			newline();
	}
  
	if(finnouMode)
		buffer_video->attribute = 0x0F & (buffer_video->attribute + 13 % 8);

	updateCursorPosition();
}

void kprintf(const char *format, ...) {
  char **arg = (char **) &format;
  int c;
  char buf[20];

  arg++;

  while ((c = *format++) != 0)
    {
      if (c != '%')
        kputchar (c);
      else
        {
          char *p;

          c = *format++;
          switch (c)
            {
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
                kputchar (*p++);
              break;

            default:
              kputchar (*((int *) arg++));
              break;
            }
        }
    }
}

/* Fonction temporaire !!!! */
size_t write_screen(const void *buf, size_t count) {
	size_t i;
	for (i = 0; i < count && *(char *)buf != '\0'; i++) {
		kputchar(*(char *)buf);
		buf++;
	}
	return count;
}

void enableFinnouMode(int enable)
{
  finnouMode = enable;
}

void set_foreground(uint8_t foreground) {
  buffer_video->attribute = buffer_video->attribute & 0xF0 | (foreground & 0xF); 
}

void set_background(uint8_t background) {
  buffer_video->attribute = ((background & 0xF) << 4) | buffer_video->attribute & 0x0F;
}

void set_attribute(uint8_t background, uint8_t foreground)
{
  buffer_video->attribute = ((background & 0xF) << 4) | (foreground & 0xF); 
}

void set_attribute_position(uint8_t background, uint8_t foreground, int x, int y)
{
	buffer_video->buffer[x + ((y+buffer_video->bottom_buffer)%LINES) * COLUMNS].attribute = buffer_video->attribute;
	(*video)[x + y * COLUMNS].attribute = ((background & 0xF) << 4) | (foreground & 0xF); 
}

void reset_attribute()
{
  buffer_video->attribute = DEFAULT_ATTRIBUTE_VALUE;
}

