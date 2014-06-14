/**
 * @file console.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2014 TacOS developers.
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
 * @brief Console virtuelle (Fn).
 */

#include <drivers/console.h>
#include <drivers/video.h>
#include <kmalloc.h>
#include <klog.h>

#define LARGEUR_TAB 8 /**< Taille d'une tabulation */

#define NB_CONSOLES 4 /**< Nombre de consoles virtuelles */

static tty_driver_t *tty_driver;

static struct console_t consoles[NB_CONSOLES];

static size_t console_write (tty_struct_t* tty, open_file_descriptor* ofd, const unsigned char* buf, size_t count);
static void console_putchar(tty_struct_t *tty, unsigned char c);
static void clear_console(int n);
static void backspace(int n);
static void kputchar_tab(tty_struct_t *tty);

static int active_console = 0;

static tty_operations_t console_ops = {.open=NULL, .close=NULL, .write=console_write, .put_char=console_putchar, .set_termios=NULL, .ioctl=NULL};

void console_init() {
	int i;
	for (i = 0; i < NB_CONSOLES; i++) {
		consoles[i].used = false;
		consoles[i].lines = 25;
		consoles[i].cols = 80;
		consoles[i].attr = DEFAULT_ATTRIBUTE_VALUE;
		consoles[i].disp_cur = true;
		consoles[i].cur_x = 0;
		consoles[i].cur_y = 0;
	
		consoles[i].escape_char = false;
		consoles[i].ansi_escape_code = false;
		consoles[i].ansi_second_val = false;
		consoles[i].val = 0;
		consoles[i].val2 = 0;
		consoles[i].bright = 1;

		clear_console(i);
	}

	/* Enregistre le driver */
	tty_driver = alloc_tty_driver(NB_CONSOLES);
	tty_driver->driver_name = "console";
	tty_driver->devfs_name = "tty";
	tty_driver->type = TTY_DRIVER_TYPE_CONSOLE;
	tty_driver->init_termios = tty_std_termios;
	tty_driver->ops = &console_ops;
	tty_register_driver(tty_driver);
}

tty_struct_t* get_active_terminal() {
	return tty_driver->ttys[active_console];
}

void focus_console(int n) {
	active_console = n;
	switch_page(n);
	disable_cursor(!consoles[n].disp_cur);
	if (consoles[n].disp_cur) {
		cursor_position_video(n, consoles[n].cur_x, consoles[n].cur_y);
	}
}

static void cursor_up(int n, unsigned int count) {
	if (consoles[n].cur_y > count)
		consoles[n].cur_y -= count;
	else
		consoles[n].cur_y = 0;
}

static void cursor_down(int n, unsigned int count) {
	if (consoles[n].cur_y + count < consoles[n].lines)
		consoles[n].cur_y += count;
	else
		consoles[n].cur_y = consoles[n].lines - 1;
}

static void cursor_back(int n, unsigned int count) {
	if (consoles[n].cur_x > count)
		consoles[n].cur_x -= count;
	else
		consoles[n].cur_x = 0;
}

static void cursor_forward(int n, unsigned int count) {
	if (consoles[n].cur_x + count < consoles[n].cols)
		consoles[n].cur_x += count;
	else
		consoles[n].cur_x = consoles[n].cols - 1;
}

static void lineup(int n, unsigned int count) {
	cursor_up(n, count);
	consoles[n].cur_x = 0;
}

static void cursor_move_col(unsigned int n, unsigned int x) {
	if (x <= consoles[n].cols) {
		consoles[n].cur_x = x - 1;
	}
}

static void cursor_move(unsigned int n, unsigned int y, unsigned int x) {
	if (y > 0 && y <= consoles[n].lines) {
		consoles[n].cur_y = y - 1;
	}
	if (x > 0 && x <= consoles[n].cols) {
		consoles[n].cur_x = x - 1;
	}
}

static void clear_console(int n) {
	unsigned int x, y;

	for (y = 0; y < consoles[n].lines; y++) {
		for (x = 0; x < consoles[n].cols; x++) {
			kputchar_video(n, false, ' ', x, y,
					consoles[n].attr);
		}
	}
	flip_page(n);

	consoles[n].cur_x = 0;
	consoles[n].cur_y = 0;
	cursor_position_video(n, 0, 0);
}


static void newline(int n) {
	consoles[n].cur_x = 0;
	consoles[n].cur_y++;
	if (consoles[n].cur_y >= consoles[n].lines) {
		scrollup(n, consoles[n].attr);
		consoles[n].cur_y--;
	}
	cursor_position_video(n, consoles[n].cur_x, consoles[n].cur_y);
}

static void set_foreground(int n, uint8_t foreground) {
	consoles[n].attr = (consoles[n].attr & 0xF0) | (foreground & 0xF);
}

static void set_background(int n, uint8_t background) {
	consoles[n].attr = ((background & 0xF) << 4) | (consoles[n].attr & 0x0F);
}

static void reset_attribute(int n) {
	consoles[n].attr = DEFAULT_ATTRIBUTE_VALUE;
}

static size_t console_write (tty_struct_t* tty, open_file_descriptor* ofd __attribute__ ((unused)), const unsigned char* buf, size_t count) {
	size_t i;
	for (i = 0; (i < count && buf[i] != '\0'); i++) {
		console_putchar(tty, buf[i]);
	}
	return i;
}

/*
 *  Affiche le caractère c sur l'écran.
 *  Supporte les caractères ANSI.
 */
static void console_putchar(tty_struct_t *tty, unsigned char c) {
	int n = tty->index;

	if (consoles[n].escape_char) {
		if (consoles[n].ansi_escape_code) {
			if (c >= '0' && c <= '9') {
				if (consoles[n].ansi_second_val) {
					consoles[n].val2 = consoles[n].val2 * 10 + c - '0';
				} else {
					consoles[n].val = consoles[n].val * 10 + c - '0';
				}
			} else {
				consoles[n].escape_char = false;
				consoles[n].ansi_second_val = false;
				consoles[n].ansi_escape_code = false;
				if (consoles[n].val == 0 && c != 'J' && c != 'm')
					consoles[n].val = 1;
				if (consoles[n].val2 == 0)
					consoles[n].val2 = 1;
				switch (c) {
				case 'A':
					cursor_up(n, consoles[n].val);
					break;
				case 'B':
					cursor_down(n, consoles[n].val);
					break;
				case 'C':
					cursor_forward(n, consoles[n].val);
					break;
				case 'D':
					cursor_back(n, consoles[n].val--);
					break;
				case 'E':
					while (consoles[n].val--)
						newline(n);
					break;
				case 'F':
					lineup(n, consoles[n].val);
					break;
				case 'G':
					cursor_move_col(n, consoles[n].val);
					break;
				case 'f':
				case 'H':
					cursor_move(n, consoles[n].val, consoles[n].val2);
					break;
				case ';':
					consoles[n].escape_char = true;
					consoles[n].ansi_second_val = true;
					consoles[n].ansi_escape_code = true;
					consoles[n].val2 = 0;
					break;
				case 'm':
					if (consoles[n].val == 0) {
						reset_attribute(n);
					} else if (consoles[n].val == 1) {
						consoles[n].bright = 1;
					} else if (consoles[n].val == 2) {
						consoles[n].bright = 0;
					} else if (consoles[n].val >= 30 && consoles[n].val <= 37) {
						// si low intensity (normal) :
						if (consoles[n].bright == 0) {
							switch (consoles[n].val - 30) {
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
								set_foreground(n, BROWN);
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
								set_foreground(n, LIGHT_GRAY);
								break;
							}
						} else {
							switch (consoles[n].val - 30) {
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
					} else if (consoles[n].val >= 40 && consoles[n].val <= 47) {
						// low intensity (normal) :
						switch (consoles[n].val - 40) {
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
							set_background(n, BROWN);
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
							set_background(n, LIGHT_GRAY);
							break;
						}
					} else if (consoles[n].val >= 90 && consoles[n].val <= 99) {
						switch (consoles[n].val - 90) {
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
					} else if (consoles[n].val >= 100 && consoles[n].val <= 109) {
						switch (consoles[n].val - 100) {
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
					break;
				case 'J':
					if (consoles[n].val == 0) {

					} else if (consoles[n].val == 1) {

					} else if (consoles[n].val == 2) {
						clear_console(n);
					}
					break;
				}
			}
		} else {
			if (c == '[') {
				consoles[n].ansi_escape_code = true;
				consoles[n].val = 0;
			} else {
				consoles[n].escape_char = false;
			}
		}
	} else if (c == '\033') {
		consoles[n].escape_char = true;
	} else if (c == '\n' || c == '\r') {
		newline(n);
	} else if (c == '\t') {
		kputchar_tab(tty);
	} else if (c == '\b') {
		backspace(n);
	} else if (c > 31 && c < 255) {
		if (consoles[n].cur_x >= consoles[n].cols) {
			newline(n);
		}
		kputchar_video(n, true, c, consoles[n].cur_x, consoles[n].cur_y,
										consoles[n].attr);
		consoles[n].cur_x++;
	}

	cursor_position_video(n, consoles[n].cur_x, consoles[n].cur_y);
}

static void kputchar_tab(tty_struct_t *tty) {
	int n = tty->index;
	unsigned int x = consoles[n].cur_x;

	x = ((x / LARGEUR_TAB) + 1) * LARGEUR_TAB;
	if (x >= consoles[n].cols) {
		newline(tty->index);
	} else {
		while (consoles[n].cur_x < x) {
			console_putchar(tty, ' ');
		}
	}
}

/**
 *	Suppression en arrière d'un caractère.
 */
static void backspace(int n) {
	if (consoles[n].cur_x > 0) {
		consoles[n].cur_x--;
	} else if (consoles[n].cur_y > 0) {
		consoles[n].cur_y--;
		consoles[n].cur_x = consoles[n].cols - 1;
	}
}
