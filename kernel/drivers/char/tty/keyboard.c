/**
 * @file keyboard.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
 * @brief Driver clavier.
 */

#include <types.h>
#include <ioports.h>
#include <drivers/keyboard.h>
#include <drivers/console.h>
#include <klog.h>

/**
 * Scancode SET 1 : (IBM XT)
 * http://www.computer-engineering.org/ps2keyboard/scancodes1.html
 */

/** SCAN CODES **/

#define KEY_RELEASE 0x80 /**< Masque pour le scancode d'un relâchement de touche. */

// Protocol 
#define KEYBOARD_ERROR_MODE_2 0x00 /**< Erreur en scancode mode 2 et 3 */
#define KEYBOARD_BAT_OK       0xAA /**< Basic Assurance Test */
#define KEYBOARD_ECHO_RES     0xEE /**< Result of echo command */
#define KEYBOARD_UNKNOWN      0xF1 /**< ???? */
#define KEYBOARD_BAT_KO       0xFC /**< Basic Assurance Test */
#define KEYBOARD_FAILURE      0xFD /**< Internal failure */
#define KEYBOARD_ERROR_MODE_1 0xFF /**< Erreur en scancode mode 1 */
// Escape
#define KEYBOARD_EXPAND   0xE0 /**< scancode sur 2 octets. */
#define KEYBOARD_PAUSE    0xE1 /**< scancode sur 3 octets. */

#define KEY_ESCAPE  0x01 /**< scancode escape key. */
#define KEY_ALT     0x38 /**< scancode alt key. */
#define KEY_LSHIFT  0x2A /**< scancode left shift key. */
#define KEY_RSHIFT  0x36 /**< scancode right shift key. */
#define KEY_LCTRL	0x1D /**< scancode left control key. */
#define KEY_CAPSLOCK 0x3A /**< scancode capslock key. */
#define KEY_NUMLOCK 0x45 /**< scancode numlock key. */ 
#define KEY_RETURN  0x1C /**< scancode return key. */
#define KEY_SPACE   0x39 /**< scancode space key. */
#define KEY_F1      0x3b /**< scancode F1 key. */
#define KEY_F2      0x3c /**< scancode F2 key. */
#define KEY_F3      0x3d /**< scancode F3 key. */
#define KEY_F4      0x3e /**< scancode F4 key. */
#define KEY_F5      0x3f /**< scancode F5 key. */
#define KEY_F6      0x40 /**< scancode F6 key. */
#define KEY_F7      0x41 /**< scancode F7 key. */
#define KEY_F8      0x42 /**< scancode F8 key. */
#define KEY_F9      0x43 /**< scancode F9 key. */
#define KEY_F10     0x44 /**< scancode F10 key. */
#define KEY_F11     0x57 /**< scancode F11 key. */
#define KEY_F12     0x58 /**< scancode F12 key. */

/**
 * Structure qui contient les charactères ANSI pour un keycode en fonction
 * des modifiers.
 */
struct symbole {
	const char* normal; /**< Code normal. */
	const char* caps; /**< Code si shift appuyé ou capslock. */
	const char* alt; /**< Code si la touche alt de gauche est appuyée. */
	const char* ctrl; /**< Code si la touche ctrl est appuyée. */
	const char* alt_r; /**< Code si la touche alt_gr est appuyée. */
	const char* num_lock; /** Code si numlock actif. */
};

#define TAB_KEY 	"\x09"			/**< ANSI code for tab key. */
#define ENTER_KEY 	"\x0d"			/**< ANSI code for enter key. */
#define ESC_KEY 	"\x1b"			/**< ANSI code for escape key. */
#define BS_KEY 		"\x7f"			/**< ANSI code for backspace key. */
#define LEFT_KEY 	"\x1b\x5b\x44"		/**< ANSI code for left key. */
#define RIGHT_KEY	"\x1b\x5b\x43"		/**< ANSI code for right key. */
#define UP_KEY 		"\x1b\x5b\x41"		/**< ANSI code for up key. */
#define DOWN_KEY 	"\x1b\x5b\x42"		/**< ANSI code for down key. */
#define HOME_KEY 	"\x1b\x5b\x31\x7e"	/**< ANSI code for home key. */
#define INSERT_KEY 	"\x1b\x5b\x31\x7e"	/**< ANSI code for insert key. */
#define DEL_KEY 	"\x1b\x5b\x33\x7e"	/**< ANSI code for delete key. */
#define END_KEY 	"\x1b\x5b\x34\x7e"	/**< ANSI code for end key. */
#define PGUP_KEY 	"\x1b\x5b\x35\x7e"	/**< ANSI code for page up key. */
#define PGDOWN_KEY 	"\x1b\x5b\x36\x7e"	/**< ANSI code for page down key. */

/**
 * Keymap qwerty: keycode vers chaine ANSI.
 */
struct symbole qwerty_keymap[] = {
	{"", "", "", "", "", NULL},
	{ESC_KEY, ESC_KEY, NULL, ESC_KEY, NULL, NULL},
	{"1", "!", NULL, "1", NULL, NULL},
	{"2", "@", NULL, "2", NULL, NULL},
	{"3", "#", NULL, "3", NULL, NULL},
	{"4", "$", NULL, "4", NULL, NULL},
	{"5", "%", NULL, "5", NULL, NULL},
	{"6", "^", NULL, "6", NULL, NULL},
	{"7", "&", NULL, "7", NULL, NULL},
	{"8", "*", NULL, "8", NULL, NULL},
	{"9", "(", NULL, "9", NULL, NULL},
	{"0", ")", NULL, "0", NULL, NULL},
	{"-", "_", NULL, "-", NULL, NULL},
	{"=", "+", NULL, "=", NULL, NULL},
	{BS_KEY, BS_KEY, NULL, BS_KEY, NULL, NULL},
	{TAB_KEY, TAB_KEY, NULL, TAB_KEY, NULL, NULL},
	{"q", "Q", NULL, NULL, NULL, NULL},
	{"w", "W", NULL, NULL, NULL, NULL},
	{"e", "E", NULL, NULL, "\351", NULL},
	{"r", "R", NULL, NULL, NULL, NULL},
	{"t", "T", NULL, NULL, NULL, NULL},
	{"y", "Y", NULL, NULL, NULL, NULL},
	{"u", "U", NULL, NULL, NULL, NULL},
	{"i", "I", NULL, NULL, NULL, NULL},
	{"o", "O", NULL, NULL, NULL, NULL},
	{"p", "P", NULL, NULL, NULL, NULL},
	{"[", "{", NULL, ESC_KEY, NULL, NULL},
	{"]", "}", NULL, NULL, NULL, NULL},
	{ENTER_KEY, ENTER_KEY, NULL, NULL, NULL, NULL},
	{"", "", "", "", "", NULL},
	{"a", "A", NULL, NULL, NULL, NULL},
	{"s", "S", NULL, NULL, NULL, NULL},
	{"d", "D", NULL, NULL, NULL, NULL},
	{"f", "F", NULL, NULL, NULL, NULL},
	{"g", "G", NULL, NULL, NULL, NULL},
	{"h", "H", NULL, NULL, NULL, NULL},
	{"j", "J", NULL, NULL, NULL, NULL},
	{"k", "K", NULL, NULL, NULL, NULL},
	{"l", "L", NULL, NULL, NULL, NULL},
	{";", ":", NULL, NULL, NULL, NULL},
	{"'", "\"", NULL, NULL, NULL, NULL},
	{"`", "~", NULL, 0, NULL, NULL},
	{"", "", "", "", "", NULL},
	{"\\", "|", NULL, NULL, NULL, NULL},
	{"z", "Z", NULL, NULL, NULL, NULL},
	{"x", "X", NULL, NULL, NULL, NULL},
	{"c", "C", NULL, NULL, NULL, NULL},
	{"v", "V", NULL, NULL, NULL, NULL},
	{"b", "B", NULL, NULL, NULL, NULL},
	{"n", "N", NULL, NULL, NULL, NULL},
	{"m", "M", NULL, NULL, NULL, NULL},
	{",", "<", NULL, "", NULL, NULL},
	{".", ">", NULL, "", NULL, NULL},
	{"/", "?", NULL, "", NULL, NULL},
	{"", "", "", "", "", NULL},
	{"*", "*", "*", "*", NULL, NULL},
	{"", "", "", "", "", NULL},
	{" ", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{HOME_KEY, NULL, "", "", "", "7"},
	{UP_KEY, NULL, "", "", "", "8"},
	{PGUP_KEY, NULL, "", "", "", "9"},
	{"-", "-", "-", "-", "-", "-"},
	{LEFT_KEY, NULL, "", "", "", "4"},
	{"", NULL, "", "", "", "5"},
	{RIGHT_KEY, NULL, "", "", "", "6"},
	{"+", "+", "+", "+", "+", "+"},
	{END_KEY, NULL, "", "", "", "1"},
	{DOWN_KEY, NULL, "", "", "", "2"},
	{PGDOWN_KEY, NULL, "", "", "", "3"},
	{INSERT_KEY, NULL, "", "", "", "0"},
	{DEL_KEY, NULL, "", "", "", "."},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"/", "/", "/", "/", "/", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{HOME_KEY, HOME_KEY, HOME_KEY, HOME_KEY, HOME_KEY, NULL},
	{UP_KEY, "", "", "", "", NULL},
	{PGUP_KEY, "", "", "", "", NULL},
	{LEFT_KEY, "", "", "", "", NULL},
	{RIGHT_KEY, "", "", "", "", NULL},
	{END_KEY, "", "", "", "", NULL},
	{DOWN_KEY, "", "", "", "", NULL},
	{PGDOWN_KEY, "", "", "", "", NULL},
	{INSERT_KEY, "", "", "", "", NULL},
	{DEL_KEY, "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
};

/**
 * Keymap azerty: keycode vers chaine ANSI.
 */
struct symbole azerty_keymap[] = {
	{"", "", "", "", "", NULL},
	{ESC_KEY, ESC_KEY, NULL, ESC_KEY, NULL, NULL},
	{"&", "1", NULL, "1", NULL, NULL},
	{"\351", "2", NULL, "2", "~", NULL},
	{"\"", "3", NULL, "3", "#", NULL},
	{"'", "4", NULL, "4", "{", NULL},
	{"(", "5", NULL, "5", "[", NULL},
	{"-", "6", NULL, "6", "|", NULL},
	{"\350", "7", NULL, "7", "`", NULL},
	{"_", "8", NULL, "8", "\\", NULL},
	{"\347", "9", NULL, "9", "^", NULL},
	{"\340", "0", NULL, "0", "@", NULL},
	{")", "\260", NULL, "-", "]", NULL},
	{"=", "+", NULL, "=", "}", NULL},
	{BS_KEY, BS_KEY, NULL, BS_KEY, NULL, NULL},
	{TAB_KEY, TAB_KEY, NULL, TAB_KEY, NULL, NULL},
	{"a", "A", NULL, NULL, NULL, NULL},
	{"z", "Z", NULL, NULL, NULL, NULL},
	{"e", "E", NULL, NULL, NULL, NULL},
	{"r", "R", NULL, NULL, NULL, NULL},
	{"t", "T", NULL, NULL, NULL, NULL},
	{"y", "Y", NULL, NULL, NULL, NULL},
	{"u", "U", NULL, NULL, NULL, NULL},
	{"i", "I", NULL, NULL, NULL, NULL},
	{"o", "O", NULL, NULL, NULL, NULL},
	{"p", "P", NULL, NULL, NULL, NULL},
	{"^", "", NULL, ESC_KEY, NULL, NULL},
	{"$", "\243", NULL, NULL, "\244", NULL},
	{ENTER_KEY, ENTER_KEY, NULL, NULL, NULL, NULL},
	{"", "", "", "", "", NULL},
	{"q", "Q", NULL, NULL, NULL, NULL},
	{"s", "S", NULL, NULL, NULL, NULL},
	{"d", "D", NULL, NULL, NULL, NULL},
	{"f", "F", NULL, NULL, NULL, NULL},
	{"g", "G", NULL, NULL, NULL, NULL},
	{"h", "H", NULL, NULL, NULL, NULL},
	{"j", "J", NULL, NULL, NULL, NULL},
	{"k", "K", NULL, NULL, NULL, NULL},
	{"l", "L", NULL, NULL, NULL, NULL},
	{"m", "M", NULL, NULL, NULL, NULL},
	{"\243", "%", NULL, NULL, NULL, NULL},
	{"\xb2", "~", NULL, 0, NULL, NULL},
	{"", "", "", "", "", NULL},
	{"*", "\265", NULL, NULL, NULL, NULL},
	{"w", "W", NULL, NULL, NULL, NULL},
	{"x", "X", NULL, NULL, NULL, NULL},
	{"c", "C", NULL, NULL, NULL, NULL},
	{"v", "V", NULL, NULL, NULL, NULL},
	{"b", "B", NULL, NULL, NULL, NULL},
	{"n", "N", NULL, NULL, NULL, NULL},
	{",", "?", NULL, "", NULL, NULL},
	{";", ".", NULL, "", NULL, NULL},
	{":", "/", NULL, "", NULL, NULL},
	{"!", "\xA7", NULL, "", NULL, NULL},
	{"", "", "", "", "", NULL},
	{"*", "*", "*", "*", NULL, NULL},
	{"", "", "", "", "", NULL},
	{" ", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{HOME_KEY, NULL, "", "", "", "7"},
	{UP_KEY, NULL, "", "", "", "8"},
	{PGUP_KEY, NULL, "", "", "", "9"},
	{"-", "-", "-", "-", "-", "-"},
	{LEFT_KEY, NULL, "", "", "", "4"},
	{"", NULL, "", "", "", "5"},
	{RIGHT_KEY, NULL, "", "", "", "6"},
	{"+", "+", "+", "+", "+", "+"},
	{END_KEY, NULL, "", "", "", "1"},
	{DOWN_KEY, NULL, "", "", "", "2"},
	{PGDOWN_KEY, NULL, "", "", "", "3"},
	{INSERT_KEY, NULL, "", "", "", "0"},
	{DEL_KEY, NULL, "", "", "", "."},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"<", ">", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"/", "/", "/", "/", "/", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{HOME_KEY, HOME_KEY, HOME_KEY, HOME_KEY, HOME_KEY, NULL},
	{UP_KEY, "", "", "", "", NULL},
	{PGUP_KEY, "", "", "", "", NULL},
	{LEFT_KEY, "", "", "", "", NULL},
	{RIGHT_KEY, "", "", "", "", NULL},
	{END_KEY, "", "", "", "", NULL},
	{DOWN_KEY, "", "", "", "", NULL},
	{PGDOWN_KEY, "", "", "", "", NULL},
	{INSERT_KEY, "", "", "", "", NULL},
	{DEL_KEY, "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
	{"", "", "", "", "", NULL},
};

static struct symbole *keymap = azerty_keymap;

void selectLayout(int use_azerty) {
	if (use_azerty) {
		keymap = azerty_keymap;
	} else {
		keymap = qwerty_keymap;
	}
}

static char scancode2keycode[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 96, 97, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 98, 0, 0, 100, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 103, 104, 0, 105, 106, 106, 0, 107, 108, 109,
	110, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int alt = 0; /**< 1 si la touche alt est enfoncée, 0 sinon. */
static int alt_r = 0; /**< 1 si la touche alt gr est enfoncée, 0 sinon. */
static int shift = 0; /**< 1 si la touche shift est enfoncée, 0 sinon. */
static int capslock = 0; /**< 1 si la les majuscules sont vérouillées, 0 sinon. */
static int numlock = 0; /**< 1 si le pavé numérique est activé, 0 sinon. */
static int ctrl = 0; /**< 1 si la touche control est activée, 0 sinon. */
static uint8_t scancode_m1 = 0;
static uint8_t scancode_m2 = 0;

static void keyBufferPush(unsigned char c) {
	//kprintf("%c %d\n", c, c);
	tty_struct_t *tty = get_active_terminal();
	tty_insert_flip_char(tty, c);
}

static void keyBufferPushWord(const char* c) {
	while (*c) {
		keyBufferPush(*c);
		c++;
	}
}

static void convertKeyCode(uint8_t keycode) {
// normal, shift/numlock, alt, ctrl, alt_r
// NULL en alt signifie : 0x1b suivit du code normal.
// NULL en ctrl signifie : normal & 0x1F
// NULL en alt_r signifie: normal
	if (keycode < 128 && keycode > 0) {
		if (numlock && keymap[keycode].num_lock != NULL) {
			keyBufferPushWord(keymap[keycode].num_lock);
		} else if (ctrl) {
			if (keymap[keycode].ctrl == NULL) {
				keyBufferPush(keymap[keycode].normal[0] & 0x1f);
			} else {
				keyBufferPushWord(keymap[keycode].ctrl);
			}
		} else if (capslock || shift) {
			keyBufferPushWord(keymap[keycode].caps);
		} else if (alt) {
			if (keymap[keycode].alt == NULL) {
				keyBufferPush(0x1b);
				keyBufferPushWord(keymap[keycode].normal);
			} else {
				keyBufferPushWord(keymap[keycode].alt);
			}
		} else if (alt_r && keymap[keycode].alt_r) {
			keyBufferPushWord(keymap[keycode].alt_r);
		} else {
			keyBufferPushWord(keymap[keycode].normal);
		}
	}
}

void keyboardInterrupt(int id __attribute__ ((unused))) {
	uint8_t scancode = inb(0x60);
	uint8_t keycode = 0;

	uint8_t scancode2 = scancode & ~KEY_RELEASE;

	if (scancode2 <= 0x5f) {
		if (scancode_m1 != KEYBOARD_EXPAND && scancode_m2 != KEYBOARD_PAUSE) {
			keycode = scancode;
		} else if (scancode_m1 == KEYBOARD_EXPAND) {
			keycode = scancode2keycode[scancode2];
		}
		if (scancode & KEY_RELEASE) {
			keycode = 0x80 | keycode;
		}
	}

	switch (scancode) {
	case KEY_ALT:
		if (scancode_m1 == KEYBOARD_EXPAND) {
			alt_r = 1;
		} else {
			alt = 1;
		}
		break;
	case KEY_ALT | KEY_RELEASE:
		alt = 0;
		alt_r = 0;
		break;
	case KEY_LSHIFT:
		shift = 1;
		break;
	case KEY_LSHIFT | KEY_RELEASE:
		shift = 0;
		break;
	case KEY_RSHIFT:
		shift = 1;
		break;
	case KEY_LCTRL:
		ctrl = 1;
		break;
	case KEY_LCTRL | KEY_RELEASE:
		ctrl = 0;
		break;
	case KEY_RSHIFT | KEY_RELEASE:
		shift = 0;
		break;
	case KEY_CAPSLOCK:
		capslock = !capslock;
		break;
	case KEY_NUMLOCK:
		numlock = !numlock;
		break;
	case KEY_F1:
		focus_console(0);
		break;
	case KEY_F2:
		focus_console(1);
		break;
	case KEY_F3:
		focus_console(2);
		break;
	case KEY_F4:
		focus_console(3);
		break;

	}

	scancode_m2 = scancode_m1;
	scancode_m1 = scancode;

	convertKeyCode(keycode);
}
