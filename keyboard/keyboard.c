#include <types.h>
#include <stdio.h>
#include <ioports.h>
#include <keyboard.h>

#define BUFFER_SIZE 256

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
#define KEY_LSHIFT  0x2A
#define KEY_RSHIFT  0x36
#define KEY_CAPSLOCK 0x3A
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

static letter letters_qwerty[] = {
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


static letter letters_azerty[] = {
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
  {0x35, '!', '§'},
  {0x56, '<', '>'}
};

static char buffer[BUFFER_SIZE];
static int begin = 0, end = 0;
static int shift = 0;
static int capslock = 0;

void keyBufferPush(char c)
{
  if((end+1 % BUFFER_SIZE) != begin)
  {
    buffer[end++] = c;
    if(end >= BUFFER_SIZE)
      end = 0;
  }
}

char keyBufferPop()
{
  char c = -1;
  if(begin != end)
  {
    c = buffer[begin++];
    if(begin >= BUFFER_SIZE)
      begin = 0;
  }
  return c;
}

char keyboardConvertToChar(uint8_t scancode) {
	unsigned int i;
   for (i=0 ; i < sizeof(letters_azerty) / sizeof(letter); i++) {
		if (scancode == letters_azerty[i].scancode) {
			if ((shift && !capslock) || (!shift && capslock)) {
				return letters_azerty[i].uppercase;
			} else {
				return letters_azerty[i].lowercase;
			}
		}
   }
	return 0;
}

void keyboardInterrupt(int id)
{
  uint8_t scancode = inb(0x60);
  switch(scancode)
  {
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
  case KEY_RETURN:
    keyBufferPush('\n');
    break;
  case KEY_SPACE:
    keyBufferPush(' ');
	 break;
  case KEY_F1:
	 switchStandardBuffer();
	 break;
  case KEY_F2:
	 switchDebugBuffer();
	 break;
  default: {
    char c = keyboardConvertToChar(scancode);
    if (c != 0) {
		 keyBufferPush(c);
	 }
			  }
  }
}

char getchar()
{
  char c;
  int i;
  
  while((c = keyBufferPop()) == -1)
  {
    // XXX : Mais pourquoi y a besoin de cette fucking tempo ?
//  for(i = 0 ; i<100000 ; i++)
      ;
	// Ca marche sans la tempo en +... ?
  }
  return c;
}

