#ifndef _VIDEO_H_
#define _VIDEO_H_

/**
 * @file video.h
 */

#include <types.h>

#define DEFAULT_ATTRIBUTE_VALUE 0x09

#define BLACK         0x0
#define BLUE          0x1
#define GREEN         0x2
#define CYAN          0x3
#define RED           0x4
#define MAGENTA       0x5
#define BROWN         0x6
#define LIGHT_GRAY    0x7
#define DARK_GRAY     0x8
#define LIGHT_BLUE    0x9
#define LIGHT_GREEN   0xA
#define LIGHT_CYAN    0xB
#define LIGHT_RED     0xC
#define LIGHT_MAGENTA 0xD
#define YELLOW        0xE
#define WHITE         0xF

/* The number of columns. */
#define COLUMNS                 80
/* The number of lines. */
#define LINES                   25

typedef struct x86_video_char {
	unsigned char character;
	unsigned char attribute;
} __attribute__ ((packed)) x86_video_mem[LINES*COLUMNS];

typedef struct {
	x86_video_mem buffer;
} buffer_video_t;



/* Défini une fenetre texte */
typedef struct {
	int x, y;
	int cols, lines;
	int cursor_x, cursor_y;
	bool disable_cursor;
	uint8_t attribute;
	int n_buffer;
	struct x86_video_char *buffer;
} text_window;

void cls (text_window *tw);
void newline();
uint8_t get_bg_position(int x, int y);
uint8_t get_fg_position(int x, int y);
void reset_attribute();
void switchBuffer(int i);
void kputchar (text_window *tw, char c);
void backspace(text_window *tw, char c);
void init_video();
void focus(text_window *tw);
text_window * creation_text_window(int x, int y, int cols, int lines, int cursor_x, int cursor_y, bool disable_cursor, uint8_t attribute, int buffer);
void resize_text_window(text_window *tw, int x, int y);
void disable_cursor(int disable);

/* Avec le syscall qui va bien : */
void set_attribute(uint8_t background, uint8_t foreground);
void set_attribute_position(uint8_t background, uint8_t foreground, int x, int y);
void* sys_video_ctl( uint32_t ctl_code ,uint32_t tw , uint32_t args );

#endif
