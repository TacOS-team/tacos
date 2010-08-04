#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <ioports.h>
#include <keyboard.h>
#include <process.h>
#include <video.h>

#define BUFFER_SIZE 256

/**
 * Scancode SET 1 : (IBM XT)
 * http://www.computer-engineering.org/ps2keyboard/scancodes1.html
 */


/** SCAN CODES **/

#define KEY_RELEASE 0x80

// Protocol 
#define KEYBOARD_ERROR_MODE_2 0x00 // Erreur en scancode mode 2 et 3
#define KEYBOARD_BAT_OK       0xAA // Basic Assurance Test
#define KEYBOARD_ECHO_RES     0xEE // Result of echo command
#define KEYBOARD_UNKNOWN      0xF1 // ????
#define KEYBOARD_ACK          0xFA
#define KEYBOARD_BAT_KO       0xFC // Basic Assurance Test
#define KEYBOARD_FAILURE      0xFD // Internal failure
#define KEYBOARD_ACK_FAIL     0xFE // fails to ack, please resend
#define KEYBOARD_ERROR_MODE_1 0xFF // Erreur en scancode mode 1

// Escape
#define KEYBOARD_EXPAND   0xE0
#define KEYBOARD_PAUSE    0xE1 // pause/break
#define KEYBOARD_LOGITECH 0xE2

#define KEY_ESCAPE  0x01
#define KEY_ALT  0x38
#define KEY_LSHIFT  0x2A
#define KEY_RSHIFT  0x36
#define KEY_CAPSLOCK 0x3A
#define KEY_NUMLOCK 0x45
#define KEY_RETURN  0x1C
#define KEY_SPACE   0x39
#define KEY_F1      0x3b
#define KEY_F2      0x3c
#define KEY_F3      0x3d
#define KEY_F4      0x3e
#define KEY_F5      0x3f
#define KEY_F6      0x40
#define KEY_F7      0x41
#define KEY_F8      0x42
#define KEY_F9      0x43
#define KEY_F10     0x44
#define KEY_F11     0x57
#define KEY_F12     0x58

typedef struct {
  uint8_t scancode;
  char lowercase;
  char uppercase;
} letter;

static letter letters_qwerty[] __attribute__ ((unused))= {
  {0x02, '1', '!'},
  {0x03, '2', '@'},
  {0x04, '3', '#'},
  {0x05, '4', '$'},
  {0x06, '5', '%'},
  {0x07, '6', '^'},
  {0x08, '7', '&'},
  {0x09, '8', '*'},
  {0x0a, '9', '('},
  {0x0b, '0', ')'},
  {0x0c, '-', '_'},
  {0x0d, '=', '+'},
  {0x10, 'q', 'Q'},
  {0x11, 'w', 'W'},
  {0x12, 'e', 'E'},
  {0x13, 'r', 'R'},
  {0x14, 't', 'T'},
  {0x15, 'y', 'Y'},
  {0x16, 'u', 'U'},
  {0x17, 'i', 'I'},
  {0x18, 'o', 'O'},
  {0x19, 'p', 'P'},
  {0x1E, 'a', 'A'}, 
  {0x1F, 's', 'S'}, 
  {0x20, 'd', 'D'}, 
  {0x21, 'f', 'F'}, 
  {0x22, 'g', 'G'}, 
  {0x23, 'h', 'H'}, 
  {0x24, 'j', 'J'}, 
  {0x25, 'k', 'K'}, 
  {0x26, 'l', 'L'}, 
  {0x2C, 'z', 'Z'}, 
  {0x2D, 'x', 'X'}, 
  {0x2E, 'c', 'C'}, 
  {0x2F, 'v', 'V'}, 
  {0x30, 'b', 'B'}, 
  {0x31, 'n', 'N'}, 
  {0x32, 'm', 'M'} 
};


static letter letters_azerty[] = { // TODO : remplacer le scancode par un keycode.
 	{0x02, '&', '1'},
	{0x03, 130, '2'},
	{0x04, '"', '3'},
	{0x05, '\'', '4'},
	{0x06, '(', '5'},
	{0x07, '-', '6'},
	{0x08, 138, '7'},
	{0x09, '_', '8'},
	{0x0a, 135, '9'},
	{0x0b, 133, '0'},
	{0x0c, ')', 167},
	{0x0d, '=', '+'},
	{0x0e, '\b', '\b'},
	{0x0f, '\t', '\t'},
	{0x10, 'a', 'A'},
	{0x11, 'z', 'Z'},
	{0x12, 'e', 'E'},
	{0x13, 'r', 'R'},
	{0x14, 't', 'T'},
	{0x15, 'y', 'Y'},
	{0x16, 'u', 'U'},
	{0x17, 'i', 'I'},
	{0x18, 'o', 'O'},
	{0x19, 'p', 'P'},
	{0x1B, '$', 156}, 
	{0x1E, 'q', 'Q'}, 
	{0x1F, 's', 'S'}, 
	{0x20, 'd', 'D'}, 
	{0x21, 'f', 'F'}, 
	{0x22, 'g', 'G'}, 
	{0x23, 'h', 'H'}, 
	{0x24, 'j', 'J'}, 
	{0x25, 'k', 'K'}, 
	{0x26, 'l', 'L'}, 
	{0x27, 'm', 'M'},
	{0x28, 151, '%'}, 
	{0x2B, '*', 230}, 
	{0x2C, 'w', 'W'}, 
	{0x2D, 'x', 'X'}, 
	{0x2E, 'c', 'C'}, 
	{0x2F, 'v', 'V'}, 
	{0x30, 'b', 'B'}, 
	{0x31, 'n', 'N'}, 
	{0x32, ',', '?'},
	{0x33, ';', '.'},
	{0x34, ':', '/'},
	{0x35, '!', ' '},
	{0x56, '<', '>'},
	{0x52, '0', '0'},
	{0x4F, '1', '1'},
	{0x50, '2', '2'},
	{0x51, '3', '3'},
	{0x4B, '4', '4'},
	{0x4C, '5', '5'},
	{0x4D, '6', '6'},
	{0x47, '7', '7'},
	{0x48, '8', '8'},
	{0x49, '9', '9'}
};

static int alt = 0;
static int shift = 0;
static int capslock = 0;
static int numlock = 0;
static uint8_t scancode_m1 = 0;
static uint8_t scancode_m2 = 0;

void keyBufferPush(char c)
{
	process_t *process = get_active_process();

	open_file_descriptor *ofd;

	if (process->fd[0].used) {
		ofd = process->fd[0].ofd;

		ofd->write(ofd, (const void*)(&c), 1);
	}
}

size_t write_keyboard(open_file_descriptor *ofd, const void *buf, size_t count) {
	if (ofd != NULL) {
        if (((char*)buf)[0] == '\b') {
            if (ofd->current_octet_buf) {
        		ofd->current_octet_buf--;
                printf("\b");
                fflush(stdout);
            }
        } else {
    		ofd->buffer[ofd->current_octet_buf++] = ((char*)buf)[0];
            printf("%c", ((char*)buf)[0]);
            fflush(stdout);
        }
	}
	return count;
}

char keyboardConvertToChar(uint8_t scancode) {
	unsigned int i;
   for (i=0 ; i < sizeof(letters_azerty) / sizeof(letter); i++) {
		if (scancode == letters_azerty[i].scancode) {
			if ((shift && !capslock) || (!shift && capslock) || (numlock && scancode >= 0x47 && scancode <= 0x52)) {
				return letters_azerty[i].uppercase;
			} else {
				return letters_azerty[i].lowercase;
			}
		}
   }
	return 0;
}

void keyboardInterrupt(int id __attribute__ ((unused)))
{
	uint8_t scancode = inb(0x60);
	
	switch(scancode)
	{
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
		// XXX : switchBuffer ne devrait plus être ici je pense... Car il faudrait aussi changer le buffer au premier plan.
		switchBuffer(0);
		break;
	case KEY_F2:
		switchBuffer(1);
		break;
	default: 
		if (alt) {
			if (scancode == 0xf) {
				change_active_process();
			}
		} else if (scancode_m1 != 0xe0 && scancode_m2 != 0xe1) {
			char c = keyboardConvertToChar(scancode);
			if (c != 0) {
				keyBufferPush(c);
			}
		}
			  
	}

	scancode_m2 = scancode_m1;
	scancode_m1 = scancode;
}

