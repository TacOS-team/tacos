#include <types.h>
#include <ioports.h>
#include <ctype.h>
#include <video.h>
#include <stdlib.h>
#include <syscall.h>
#include <process.h>

/* The video memory address. */
#define VIDEO                   0xB8000

#define LARGEUR_TAB 8

#define CRT_REG_INDEX 0x3d4
#define CRT_REG_DATA  0x3d5
#define CURSOR_POS_MSB 0x0E
#define CURSOR_POS_LSB 0x0F

#define CTL_COL 0
#define CTL_COL_POS 1

/** The base pointer for the video memory */
/* volatile pour éviter des problèmes d'optimisation de gcc. */
static volatile x86_video_mem *video = (volatile x86_video_mem*)VIDEO;
static buffer_video_t buffer[2];
static buffer_video_t *buffer_video = &buffer[0];

static text_window *tw_focus = NULL;

static void updateCursorPosition();
void set_foreground(text_window * tw, uint8_t foreground);
void set_background(text_window * tw, uint8_t background);


static void kputchar_position(int n, char c, int x, int y, int attribute) {
	if (x < COLUMNS && y < LINES) {
		buffer[n].buffer[x + y * COLUMNS].character = c & 0xFF;
		buffer[n].buffer[x + y * COLUMNS].attribute = attribute;
		if (buffer_video == &buffer[n]) {
			(*video)[x + y * COLUMNS].character = c & 0xFF;
			(*video)[x + y * COLUMNS].attribute = attribute;
		}
	}
}

static void refresh(text_window *tw) {
	int i,x,y;

	if (tw == NULL) {
		for (i = 0; i < COLUMNS * LINES; i++) {
		  (*video)[i].character = buffer_video->buffer[i].character;
		  (*video)[i].attribute = buffer_video->buffer[i].attribute;
		}
	} else {
		for (y = 0; y < tw->lines; y++) {
			for (x = 0; x < tw->cols; x++) {
				kputchar_position(tw->n_buffer, tw->buffer[y * tw->cols + x].character, tw->x + x, tw->y + y, tw->buffer[y * tw->cols + x].attribute);
			}
		}
	}
}

static void kputchar_position_tw(text_window *tw, char c, int x, int y, int attribute) {
//	process_t *active_process = get_active_process();
//	if (active_process) {
//		text_window *tw_focus = (text_window *)(active_process->fd[1].ofd->extra_data);
		if (tw_focus == NULL || tw == tw_focus || tw->x + x < tw_focus->x || tw->x + x >=  tw_focus->x + tw_focus->cols || tw->y + y < tw_focus->y || tw->y + y >= tw_focus->y + tw_focus->lines || tw->n_buffer == 1)
			kputchar_position(tw->n_buffer, c, tw->x + x, tw->y + y, attribute);
//	}
	tw->buffer[x + y * tw->cols].character = c;
	tw->buffer[x + y * tw->cols].attribute = attribute;
}



/* Clear the screen and initialize VIDEO, XPOS and YPOS. */
static void clear (void) {
	int i;
 
	for (i = 0; i < COLUMNS * LINES; i++) {
	  buffer_video->buffer[i].character = 0;
	  buffer_video->buffer[i].attribute = DEFAULT_ATTRIBUTE_VALUE;
	}

	refresh(NULL);
}

void init_video() {
	reset_attribute();
	clear();
}

void focus(text_window *tw) {
	tw_focus = tw;
	
	switchBuffer(tw->n_buffer);
	refresh(tw);
	if (tw->disable_cursor) {
		disableCursor(tw);
	} else {
		updateCursorPosition(tw);
	}
}

void cls(text_window *tw) {
	int x,y;
 
	for (y = 0; y < tw->lines; y++) {
		for (x = 0; x < tw->cols; x++) {
			kputchar_position_tw(tw, ' ', x, y, tw->attribute);
		}
	}

	tw->cursor_x = 0;
	tw->cursor_y = 0;
	updateCursorPosition(tw);
}

void switchBuffer(int i) { 	
	if (buffer_video != &buffer[i]) {
		buffer_video = &buffer[i];
		refresh(NULL);
	}
}

static void scrollup(text_window *tw) {
	int l, c;

	for (l = 0; l < tw->lines - 1; l++) {
		for (c = tw->x; c < tw->cols; c++) {
			kputchar_position_tw(tw, tw->buffer[c + (l + 1) * tw->cols].character, c, l, tw->buffer[c + (l + 1) * tw->cols].attribute);
		}
	}

	/*
	 * On met des espaces sur la dernière ligne
	 */
	for (c = 0; c < tw->cols; c++) {
		kputchar_position_tw(tw, ' ', c, tw->lines - 1, tw->attribute);
	}

	refresh(tw);
}

static void updateCursorPosition(text_window * tw)
{
  int pos = tw->x + tw->cursor_x + (tw->y + tw->cursor_y)*COLUMNS;

  outb(CURSOR_POS_LSB, CRT_REG_INDEX);
  outb((uint8_t) pos, CRT_REG_DATA);
  outb(CURSOR_POS_MSB, CRT_REG_INDEX);
  outb((uint8_t) (pos >> 8), CRT_REG_DATA);
}

void disableCursor(text_window * tw)
{
  /* CRT index port => ask for access to register 0xa ("cursor
	   start") */
	outb(0x0a, CRT_REG_INDEX);

	/* (RBIL Tables 708 & 654) CRT Register 0xa => bit 5 = cursor OFF */
	outb(1 << 5, CRT_REG_DATA);

	tw->disable_cursor = 1;
}

void enableCursor(text_window * tw)
{
	tw->disable_cursor = 0;
	updateCursorPosition(tw);
}

void newline(text_window * tw)
{
	tw->cursor_x = 0;
	tw->cursor_y++;
	if (tw->cursor_y >= tw->lines) {
		scrollup(tw);
		tw->cursor_y--;
	}
	updateCursorPosition(tw);
}

void kputchar_tab(text_window * tw) {
	int x = tw->cursor_x;
	
	x = ((x / LARGEUR_TAB) + 1) * LARGEUR_TAB;
	if (x >= tw->cols) {
		newline(tw);
	} else {
		while(tw->cursor_x < x) {
			kputchar(tw, ' ');
		}
	}
}

void cursor_up(text_window * tw) {
	if (tw->cursor_y > 0)
		tw->cursor_y--;
	updateCursorPosition(tw);
}

void cursor_down(text_window * tw) {
	if (tw->cursor_y < tw->lines - 1)
		tw->cursor_y++;
	updateCursorPosition(tw);
}

void cursor_back(text_window * tw) {
	if (tw->cursor_x > 0)
		tw->cursor_x--;
	updateCursorPosition(tw);
}

void cursor_forward(text_window * tw) {
	if (tw->cursor_x < tw->cols -1)
		tw->cursor_x++;
	updateCursorPosition(tw);
}

void lineup(text_window * tw) {
	cursor_up(tw);
	tw->cursor_x = 0;
	updateCursorPosition(tw);
}

void cursor_move_col(text_window * tw, int n) {
	if (n <= tw->cols) {
		tw->cursor_x = n-1;
		updateCursorPosition(tw);
	}
}

void cursor_move(text_window * tw, int n, int m) {
	if (n > 0 && n <= tw->lines) {
		tw->cursor_y = n-1;
	}
	if (m > 0 && m <= tw->cols) {
		tw->cursor_x = m-1;
	}
}

/**
 *	Suppression en arrière d'un caractère.
 */
void backspace(text_window *tw, char c) {
	if (c == '\t') {
/*		int x = buffer_video->xpos - LARGEUR_TAB;
		int y = buffer_video->ypos;
		if (x < 0) x += COLUMNS;
		while (buffer_video->xpos > x && buffer_video->buffer[buffer_video->xpos + ((y+buffer_video->bottom_buffer)%LINES) * COLUMNS].character == ' ') {
			buffer_video->xpos--;
		}*/
	} else {
		if (tw->cursor_x > 0) {
			tw->cursor_x--;
		} else if (tw->cursor_y > 0) {
			tw->cursor_y--;
			tw->cursor_x = tw->cols - 1;
		}

		int x = tw->cursor_x;
		int y = tw->cursor_y;

		kputchar_position_tw(tw, ' ', x, y, tw->attribute); 
	}
	updateCursorPosition(tw);
}

/*
 *  Affiche le caractère c sur l'écran.
 *  Supporte les caractères ANSI.
 */
void kputchar (text_window * tw, char c) {
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
				if (val == 0 && c != 'J' && c != 'm') val = 1;
				if (val2 == 0) val2 = 1;
				switch(c) {
					case 'A':
						while (val--) cursor_up(tw);
						break;
					case 'B':
						while (val--) cursor_down(tw);
						break;
					case 'C':
						while (val--) cursor_forward(tw);
						break;
					case 'D': 
						while (val--) cursor_back(tw);
						break;
					case 'E':
						while (val--) newline(tw);
						break;
					case 'F':
						while (val--) lineup(tw);
						break;
					case 'G':
						cursor_move_col(tw, val);
						break;
					case 'f':
					case 'H':
						cursor_move(tw, val, val2);
						break;
					case ';':
						escape_char = TRUE;
						ansi_second_val = TRUE;
						ansi_escape_code = TRUE;
						val2 = 0;
						break;
					case 'm':
						if (val == 0) {
							reset_attribute(tw);
						} else if (val >= 30 && val <= 37) {
							switch(val - 30) {
								// si low intensity (normal) :
								case 0: set_foreground(tw, BLACK);
										  break;
								case 1: set_foreground(tw, RED);
										  break;
								case 2: set_foreground(tw, GREEN);
										  break;
								case 3: set_foreground(tw, YELLOW);
										  break;
								case 4: set_foreground(tw, BLUE);
										  break;
								case 5: set_foreground(tw, MAGENTA);
										  break;
								case 6: set_foreground(tw, CYAN);
										  break;
								case 7: set_foreground(tw, WHITE); // Devrait être LIGHT_GRAY. Le White c'est pour le high intensity.
										  break;
							}
						} else if (val >= 40 && val <= 47) {
							switch(val - 40) {
								// si low intensity (normal) :
								case 0: set_background(tw, BLACK);
										  break;
								case 1: set_background(tw, RED);
										  break;
								case 2: set_background(tw, GREEN);
										  break;
								case 3: set_background(tw, YELLOW);
										  break;
								case 4: set_background(tw, BLUE);
										  break;
								case 5: set_background(tw, MAGENTA);
										  break;
								case 6: set_background(tw, CYAN);
										  break;
								case 7: set_background(tw, WHITE); // Devrait être LIGHT_GRAY. Le White c'est pour le high intensity.
										  break;
							}

						}
						break;
					case 'J':
						if (val == 0) {
							
						} else if (val == 1) {

						} else if (val == 2) {
							cls(tw);
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
		newline(tw);
	} else if (c == '\t') {
		kputchar_tab(tw);
	} else {
		kputchar_position_tw(tw, c, tw->cursor_x, tw->cursor_y, tw->attribute);
	  	tw->cursor_x++;
		if (tw->cursor_x >= tw->cols)
			newline(tw);
	}
  
	updateCursorPosition(tw);
}

void set_foreground(text_window * tw, uint8_t foreground) {
	tw->attribute = (tw->attribute & 0xF0) | (foreground & 0xF); 
}

void set_background(text_window * tw, uint8_t background) {	
	tw->attribute = ((background & 0xF) << 4) | (tw->attribute & 0x0F);
}

void sys_set_attribute(text_window * tw, uint8_t background, uint8_t foreground)
{
	tw->attribute = ((background & 0xF) << 4) | (foreground & 0xF); 
}

uint8_t get_bg_position(int x, int y)
{
	return ((*video)[x + y * COLUMNS].attribute & 0xF0) >> 4;
}

uint8_t get_fg_position(int x, int y)
{
	return (*video)[x + y * COLUMNS].attribute & 0x0F;
}

void sys_set_attribute_position(text_window *tw, uint8_t background, uint8_t foreground, int x, int y)
{
	switchBuffer(tw->n_buffer);
	buffer_video->buffer[x+tw->x + (tw->y+y) * COLUMNS].attribute = tw->attribute;
	(*video)[tw->x + x + (tw->y + y) * COLUMNS].attribute = ((background & 0xF) << 4) | (foreground & 0xF); 
}

void reset_attribute(text_window *tw)
{
  tw->attribute = DEFAULT_ATTRIBUTE_VALUE;
}

text_window * creation_text_window(int x, int y, int cols, int lines, int cursor_x, int cursor_y, bool disable_cursor, uint8_t attribute, int buffer)
{
	text_window *tw = malloc(sizeof(text_window));

	tw->x = x;
	tw->y = y;
	tw->cols = cols;
	tw->lines = lines;
	tw->cursor_x = cursor_x;
	tw->cursor_y = cursor_y;
	tw->disable_cursor = disable_cursor;
	tw->attribute = attribute;
	tw->n_buffer = buffer;
	tw->buffer = malloc(cols * lines * sizeof(struct x86_video_char));

	cls(tw);

	return tw;
}

void set_attribute(uint8_t background, uint8_t foreground) 
{
	text_window * tw = get_current_process()->fd[1].ofd->extra_data;
	uint32_t args[2];
	args[0] = background;
	args[1] = foreground;
	syscall(SYS_VIDEO_CTL,CTL_COL,(uint32_t)tw, (uint32_t)args);
}


void set_attribute_position(uint8_t background, uint8_t foreground, int x, int y) 
{
	text_window * tw = get_current_process()->fd[1].ofd->extra_data;
	uint32_t args[4];
	args[0] = background;
	args[1] = foreground;
	args[2] = x;
	args[3] = y;
	syscall(SYS_VIDEO_CTL,CTL_COL_POS,(uint32_t)tw, (uint32_t)args);
}

void* sys_video_ctl( uint32_t ctl_code ,uint32_t tw , uint32_t args )
{
	switch(ctl_code)
	{
		case CTL_COL :
			sys_set_attribute((text_window*)tw, *((uint32_t*)args), *((uint32_t*)args+1));
			break;
		case CTL_COL_POS :
			sys_set_attribute_position((text_window*)tw,
												*((uint32_t*)args), *((uint32_t*)args+1),
												*((uint32_t*)args+2), *((uint32_t*)args+3));
			break;
		default :
			break;
	}

	return 0;
}
