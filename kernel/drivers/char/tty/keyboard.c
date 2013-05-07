/**
 * @file keyboard.c
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012, 2013 - TacOS developers.
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
 * @brief Structure permettant la conversion d'un scancode vers une lettre
 * ASCII.
 * TODO: On devrait passer par une étape de keycode.
 */
typedef struct {
	uint8_t scancode; /**< Le scancode. */
	char lowercase; /**< Caractère ASCII en lowercase. */
	char uppercase; /**< Caractère ASCII en uppercase. */
} letter;

static letter letters_qwerty[] __attribute__ ((unused)) = { { 0x02, '1', '!' },
		{ 0x03, '2', '@' }, { 0x04, '3', '#' }, { 0x05, '4', '$' }, { 0x06,
				'5', '%' }, { 0x07, '6', '^' }, { 0x08, '7', '&' }, { 0x09,
				'8', '*' }, { 0x0a, '9', '(' }, { 0x0b, '0', ')' }, { 0x0c,
				'-', '_' }, { 0x0d, '=', '+' }, { 0x10, 'q', 'Q' }, { 0x11,
				'w', 'W' }, { 0x12, 'e', 'E' }, { 0x13, 'r', 'R' }, { 0x14,
				't', 'T' }, { 0x15, 'y', 'Y' }, { 0x16, 'u', 'U' }, { 0x17,
				'i', 'I' }, { 0x18, 'o', 'O' }, { 0x19, 'p', 'P' }, { 0x1E,
				'a', 'A' }, { 0x1F, 's', 'S' }, { 0x20, 'd', 'D' }, { 0x21,
				'f', 'F' }, { 0x22, 'g', 'G' }, { 0x23, 'h', 'H' }, { 0x24,
				'j', 'J' }, { 0x25, 'k', 'K' }, { 0x26, 'l', 'L' }, { 0x2C,
				'z', 'Z' }, { 0x2D, 'x', 'X' }, { 0x2E, 'c', 'C' }, { 0x2F,
				'v', 'V' }, { 0x30, 'b', 'B' }, { 0x31, 'n', 'N' }, { 0x32,
				'm', 'M' } };

static letter letters_azerty[] = { // TODO : remplacer le scancode par un keycode.
		{ 0x02, '&', '1' }, { 0x03, 130, '2' }, { 0x04, '"', '3' }, { 0x05,
				'\'', '4' }, { 0x06, '(', '5' }, { 0x07, '-', '6' }, { 0x08,
				138, '7' }, { 0x09, '_', '8' }, { 0x0a, 135, '9' }, { 0x0b,
				133, '0' }, { 0x0c, ')', 167 }, { 0x0d, '=', '+' }, { 0x0e,
				127, 127 }, { 0x0f, '\t', '\t' }, { 0x10, 'a', 'A' }, { 0x11,
				'z', 'Z' }, { 0x12, 'e', 'E' }, { 0x13, 'r', 'R' }, { 0x14,
				't', 'T' }, { 0x15, 'y', 'Y' }, { 0x16, 'u', 'U' }, { 0x17,
				'i', 'I' }, { 0x18, 'o', 'O' }, { 0x19, 'p', 'P' }, { 0x1B,
				'$', 156 }, { 0x1E, 'q', 'Q' }, { 0x1F, 's', 'S' }, { 0x20,
				'd', 'D' }, { 0x21, 'f', 'F' }, { 0x22, 'g', 'G' }, { 0x23,
				'h', 'H' }, { 0x24, 'j', 'J' }, { 0x25, 'k', 'K' }, { 0x26,
				'l', 'L' }, { 0x27, 'm', 'M' }, { 0x28, 151, '%' }, { 0x2B,
				'*', 230 }, { 0x2C, 'w', 'W' }, { 0x2D, 'x', 'X' }, { 0x2E,
				'c', 'C' }, { 0x2F, 'v', 'V' }, { 0x30, 'b', 'B' }, { 0x31,
				'n', 'N' }, { 0x32, ',', '?' }, { 0x33, ';', '.' }, { 0x34,
				':', '/' }, { 0x35, '!', ' ' }, { 0x56, '<', '>' }, { 0x52,
				'0', '0' }, { 0x4F, '1', '1' }, { 0x50, '2', '2' }, { 0x51,
				'3', '3' }, { 0x4B, '4', '4' }, { 0x4C, '5', '5' }, { 0x4D,
				'6', '6' }, { 0x47, '7', '7' }, { 0x48, '8', '8' }, { 0x49,
				'9', '9' } };

static int alt = 0; /**< 1 si la touche alt est enfoncée, 0 sinon. */
static int shift = 0; /**< 1 si la touche shift est enfoncée, 0 sinon. */
static int capslock = 0; /**< 1 si la les majuscules sont vérouillées, 0 sinon. */
static int numlock = 0; /**< 1 si le pavé numérique est activé, 0 sinon. */
static int ctrl = 0; /**< 1 si la touche control est activée, 0 sinon. */
static uint8_t scancode_m1 = 0;
static uint8_t scancode_m2 = 0;

static void keyBufferPush(char c) {
    tty_struct_t *tty = get_active_terminal();
		tty_insert_flip_char(tty, c);
}

static char keyboardConvertToChar(uint8_t scancode) {
	unsigned int i;
	for (i = 0; i < sizeof(letters_azerty) / sizeof(letter); i++) {
		if (scancode == letters_azerty[i].scancode) {
			if ((shift && !capslock) || (!shift && capslock) || (numlock
					&& scancode >= 0x47 && scancode <= 0x52)) {
				return letters_azerty[i].uppercase;
			} else {
				return letters_azerty[i].lowercase;
			}
		}
	}
	return 0;
}

void keyboardInterrupt(int id __attribute__ ((unused))) {
	uint8_t scancode = inb(0x60);

	switch (scancode) {
	case KEY_ALT:
		alt = 1;
		break;
	case KEY_ALT | KEY_RELEASE:
		alt = 0;
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
	case KEY_RETURN:
		keyBufferPush('\n');
		break;
	case KEY_SPACE:
		keyBufferPush(' ');
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
	default:
		if (ctrl) {
			char c = keyboardConvertToChar(scancode);
			if (c != 0) {
				keyBufferPush(c & 0x1F);
			}
		} else if (scancode_m1 != KEYBOARD_EXPAND && scancode_m2 != KEYBOARD_PAUSE) {
			char c = keyboardConvertToChar(scancode);
			if (c != 0) {
				keyBufferPush(c);
			}
		} else if (scancode_m1 == KEYBOARD_EXPAND) {
            switch (scancode) {
            case 0x48: // U_ARROW
                keyBufferPush(0x1B);
                keyBufferPush(0x5B);
                keyBufferPush(0x41);
                break;
            case 0x50: // D_ARROW
                keyBufferPush(0x1B);
                keyBufferPush(0x5B);
                keyBufferPush(0x42);
                break;
            case 0x4D: // R_ARROW
                keyBufferPush(0x1B);
                keyBufferPush(0x5B);
                keyBufferPush(0x43);
                break;
            case 0x4B: // L_ARROW
                keyBufferPush(0x1B);
                keyBufferPush(0x5B);
                keyBufferPush(0x44);
                break;
            case 0x49: // PG_UP
                keyBufferPush(0x1B);
                keyBufferPush(0x5B);
                keyBufferPush(0x35);
                keyBufferPush(0x7E);
                break;
            case 0x51: // PG_DN
                keyBufferPush(0x1B);
                keyBufferPush(0x5B);
                keyBufferPush(0x36);
                keyBufferPush(0x7E);
                break;
						case 0x53: // DELETE
								keyBufferPush(0x1B);
								keyBufferPush(0x5B);
								keyBufferPush(0x33);
								keyBufferPush(0x7E);
								break;
            }
        }
	}

	scancode_m2 = scancode_m1;
	scancode_m1 = scancode;
}

