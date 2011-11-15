/**
 * @file console.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#include "console.h"
#include "video.h"

#define LARGEUR_TAB 8

#define NB_CONSOLES 4
static struct console_t consoles[NB_CONSOLES];
// TODO : On pourrait changer ça par une liste de consoles pour avoir un nombre illimité.

static void clear_console(int n);
static void backspace(int n, char c);
static void init_disp_inactive_console(int n);
static void kputchar_tab(int n);

void init_console() {
	int i;
	for (i = 0; i < NB_CONSOLES; i++) {
		consoles[i].n_page = i;
		consoles[i].used = false;
		consoles[i].lines = 25;
		consoles[i].cols = 80;
		consoles[i].attr = DEFAULT_ATTRIBUTE_VALUE;
		consoles[i].disp_cur = true;
		consoles[i].cur_x = 0;
		consoles[i].cur_y = 0;
		consoles[i].tty = NULL;
		init_disp_inactive_console(i);
	}
}

static void init_disp_inactive_console(int n) {
	char *message = "Console inactive. Utilisable pour un nouveau tty !";
	int i = 0;
	clear_console(n);
	while (message[i] != '\0') {
		kputchar(n, message[i]);
		i++;
	}
}

void focus_console(int n) {
	switch_page(consoles[n].n_page);
	disable_cursor(!consoles[n].disp_cur);
	if (consoles[n].disp_cur) {
		cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
	}
	set_active_tty(consoles[n].tty);
}

int get_available_console(terminal_t *tty) {
	int i;
	for (i = 0; i < NB_CONSOLES; i++) {
		if (consoles[i].used == false) {
			consoles[i].used = true;
			consoles[i].tty = tty;
			clear_console(i);
			return i;
		}
	}
	return -1;
}

void free_console(int n) {
	consoles[n].used = false;
	consoles[n].attr = DEFAULT_ATTRIBUTE_VALUE;
	consoles[n].disp_cur = true;
	consoles[n].cur_x = 0;
	consoles[n].cur_y = 0;
	consoles[n].tty = NULL;
	init_disp_inactive_console(n);
}

static void cursor_up(int n) {
	if (consoles[n].cur_y > 0)
		consoles[n].cur_y--;
	cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
}

static void cursor_down(int n) {
	if (consoles[n].cur_y < consoles[n].lines - 1)
		consoles[n].cur_y++;
	cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
}

static void cursor_back(int n) {
	if (consoles[n].cur_x > 0)
		consoles[n].cur_x--;
	cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
}

static void cursor_forward(int n) {
	if (consoles[n].cur_x < consoles[n].cols - 1)
		consoles[n].cur_x++;
	cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
}

static void lineup(int n) {
	cursor_up(n);
	consoles[n].cur_x = 0;
	cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
}

static void cursor_move_col(unsigned int n, unsigned int x) {
	if (x <= consoles[n].cols) {
		consoles[n].cur_x = x - 1;
		cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
	}
}

static void cursor_move(unsigned int n, unsigned int y, unsigned int x) {
	if (y > 0 && y <= consoles[n].lines) {
		consoles[n].cur_y = y - 1;
	}
	if (x > 0 && x <= consoles[n].cols) {
		consoles[n].cur_x = x - 1;
	}
	cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
}

static void clear_console(int n) {
	unsigned int x, y;

	for (y = 0; y < consoles[n].lines; y++) {
		for (x = 0; x < consoles[n].cols; x++) {
			kputchar_video(consoles[n].n_page, false, ' ', x, y,
					consoles[n].attr);
		}
	}
	flip_page(consoles[n].n_page);

	consoles[n].cur_x = 0;
	consoles[n].cur_y = 0;
	cursor_position_video(0, 0);
}

static void scrollup(int n) {
	unsigned int l, c;

	for (l = 0; l < consoles[n].lines - 1; l++) {
		for (c = 0; c < consoles[n].cols; c++) {
			char car, attr;
			get_char_video(consoles[n].n_page, true, &car, c, l + 1, &attr);
			kputchar_video(consoles[n].n_page, false, car, c, l, attr);
		}
	}
	for (c = 0; c < consoles[n].cols; c++) {
		kputchar_video(consoles[n].n_page, false, ' ', c,
				consoles[n].lines - 1, consoles[n].attr);
	}

	flip_page(consoles[n].n_page);
}

static void newline(int n) {
	consoles[n].cur_x = 0;
	consoles[n].cur_y++;
	if (consoles[n].cur_y >= consoles[n].lines) {
		scrollup(n);
		consoles[n].cur_y--;
	}
	cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
}

void set_foreground(int n, uint8_t foreground) {
	consoles[n].attr = (consoles[n].attr & 0xF0) | (foreground & 0xF);
}

void set_background(int n, uint8_t background) {
	consoles[n].attr = ((background & 0xF) << 4) | (consoles[n].attr & 0x0F);
}

void sys_set_attribute(int n, uint8_t background, uint8_t foreground) {
	consoles[n].attr = ((background & 0xF) << 4) | (foreground & 0xF);
}

void reset_attribute(int n) {
	set_blink_bit(0);
	consoles[n].attr = DEFAULT_ATTRIBUTE_VALUE;
}

/*
 *  Affiche le caractère c sur l'écran.
 *  Supporte les caractères ANSI.
 */
void kputchar(int n, char c) {
	// TODO : dégager ça vers la structure de la fenetre ! Risques de conflits sinon.
	static bool escape_char = false;
	static bool ansi_escape_code = false;
	static bool ansi_second_val = false;
	static int val = 0, val2 = 0;
	static int bright;

	if (escape_char) {
		if (ansi_escape_code) {
			if (c >= '0' && c <= '9') {
				if (ansi_second_val) {
					val2 = val2 * 10 + c - '0';
				} else {
					val = val * 10 + c - '0';
				}
			} else {
				escape_char = false;
				ansi_second_val = false;
				ansi_escape_code = false;
				if (val == 0 && c != 'J' && c != 'm')
					val = 1;
				if (val2 == 0)
					val2 = 1;
				switch (c) {
				case 'A':
					while (val--)
						cursor_up(n);
					break;
				case 'B':
					while (val--)
						cursor_down(n);
					break;
				case 'C':
					while (val--)
						cursor_forward(n);
					break;
				case 'D':
					while (val--)
						cursor_back(n);
					break;
				case 'E':
					while (val--)
						newline(n);
					break;
				case 'F':
					while (val--)
						lineup(n);
					break;
				case 'G':
					cursor_move_col(n, val);
					break;
				case 'f':
				case 'H':
					cursor_move(n, val, val2);
					break;
				case ';':
					escape_char = true;
					ansi_second_val = true;
					ansi_escape_code = true;
					val2 = 0;
					break;
				case 'm':
					if (val == 0) {
						reset_attribute(n);
					} else if (val == 1) {
						bright = 1;
					} else if (val == 2) {
						bright = 0;
					} else if (val == 5 || val == 6) {
						set_blink_bit(1);
					} else if (val == 25) {
						set_blink_bit(0);
					} else if (val >= 30 && val <= 37) {
						// si low intensity (normal) :
						if (bright == 0) {
							switch (val - 30) {
							case 0:
								set_foreground(n, BLACK);
								break;
							case 1:
								set_foreground(n, RED);
								break;
							case 2:
								set_foreground(n, GREEN);
								break;
							case 3:
								set_foreground(n, YELLOW); // Devrait être BROWN
								break;
							case 4:
								set_foreground(n, BLUE);
								break;
							case 5:
								set_foreground(n, MAGENTA);
								break;
							case 6:
								set_foreground(n, CYAN);
								break;
							case 7:
								set_foreground(n, WHITE); // Devrait être LIGHT_GRAY. Le White c'est pour le high intensity.
								break;
							}
						} else {
							switch (val - 30) {
							case 0:
								set_foreground(n, DARK_GRAY);
								break;
							case 1:
								set_foreground(n, LIGHT_RED);
								break;
							case 2:
								set_foreground(n, LIGHT_GREEN);
								break;
							case 3:
								set_foreground(n, YELLOW);
								break;
							case 4:
								set_foreground(n, LIGHT_BLUE);
								break;
							case 5:
								set_foreground(n, LIGHT_MAGENTA);
								break;
							case 6:
								set_foreground(n, LIGHT_CYAN);
								break;
							case 7:
								set_foreground(n, WHITE);
								break;
							}
						}
					} else if (val >= 40 && val <= 47) {
						// si low intensity (normal) :
						if (bright == 0) {
							switch (val - 40) {
							case 0:
								set_background(n, BLACK);
								break;
							case 1:
								set_background(n, RED);
								break;
							case 2:
								set_background(n, GREEN);
								break;
							case 3:
								set_background(n, YELLOW); // Devrait être BROWN
								break;
							case 4:
								set_background(n, BLUE);
								break;
							case 5:
								set_background(n, MAGENTA);
								break;
							case 6:
								set_background(n, CYAN);
								break;
							case 7:
								set_background(n, WHITE); // Devrait être LIGHT_GRAY. Le White c'est pour le high intensity.
								break;
							}
						} else {
							switch (val - 40) {
							case 0:
								set_background(n, DARK_GRAY);
								break;
							case 1:
								set_background(n, LIGHT_RED);
								break;
							case 2:
								set_background(n, LIGHT_GREEN);
								break;
							case 3:
								set_background(n, YELLOW);
								break;
							case 4:
								set_background(n, LIGHT_BLUE);
								break;
							case 5:
								set_background(n, LIGHT_MAGENTA);
								break;
							case 6:
								set_background(n, LIGHT_CYAN);
								break;
							case 7:
								set_background(n, WHITE);
								break;
							}
						}

					}
					break;
				case 'J':
					if (val == 0) {

					} else if (val == 1) {

					} else if (val == 2) {
						clear_console(n);
					}
					break;
				}
			}
		} else {
			if (c == '[') {
				ansi_escape_code = true;
				val = 0;
			} else {
				escape_char = false;
			}
		}
	} else if (c == '\033') {
		escape_char = true;
	} else if (c == '\n' || c == '\r') {
		newline(n);
	} else if (c == '\t') {
		kputchar_tab(n);
	} else if (c == '\b') {
		backspace(n, ' ');
	} else {
		kputchar_video(consoles[n].n_page, true, c, consoles[n].cur_x,
				consoles[n].cur_y, consoles[n].attr);
		consoles[n].cur_x++;
		if (consoles[n].cur_x >= consoles[n].cols)
			newline(n);
	}

	cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
}

static void kputchar_tab(int n) {
	unsigned int x = consoles[n].cur_x;

	x = ((x / LARGEUR_TAB) + 1) * LARGEUR_TAB;
	if (x >= consoles[n].cols) {
		newline(n);
	} else {
		while (consoles[n].cur_x < x) {
			kputchar(n, ' ');
		}
	}
}

/**
 *	Suppression en arrière d'un caractère.
 */
static void backspace(int n, char c) {

	if (c == '\t') {
		/*		int x = buffer_video->xpos - LARGEUR_TAB;
		 int y = buffer_video->ypos;
		 if (x < 0) x += COLUMNS;
		 while (buffer_video->xpos > x && buffer_video->buffer[buffer_video->xpos + ((y+buffer_video->bottom_buffer)%LINES) * COLUMNS].character == ' ') {
		 buffer_video->xpos--;
		 }*/
	} else {
		if (consoles[n].cur_x > 0) {
			consoles[n].cur_x--;
		} else if (consoles[n].cur_y > 0) {
			consoles[n].cur_y--;
			consoles[n].cur_x = consoles[n].cols - 1;
		}

		int x = consoles[n].cur_x;
		int y = consoles[n].cur_y;

		kputchar_video(consoles[n].n_page, true, ' ', x, y, consoles[n].attr);
	}
	cursor_position_video(consoles[n].cur_x, consoles[n].cur_y);
}
