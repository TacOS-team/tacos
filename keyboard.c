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

#define KEY_SHIFT   0x2A
#define KEY_RETURN  0x1C
#define KEY_SPACE   0x39

typedef struct {
  uint8_t scancode;
  char lowercase;
  char uppercase;
} letter;

#define NB_LETTERS 26
static letter letters[NB_LETTERS] = {
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
  {0x32, 'm', 'M'}, 
};


static char buffer[BUFFER_SIZE];
static int begin = 0, end = 0;
static int shift = 0;

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

void keyboardInterrupt(int id)
{
  int i;
  uint8_t scancode = inb(0x60);
  switch(scancode)
  {
  case KEY_SHIFT:
    shift = 1;
    break;
  case KEY_SHIFT | KEY_RELEASE:
    shift = 0;
    break;
  case KEY_RETURN:
    keyBufferPush('\n');
    break;
  case KEY_SPACE:
    keyBufferPush(' ');
  default:
    for(i=0 ; i<NB_LETTERS ; i++)
    {
      if(scancode == letters[i].scancode) 
      {
        keyBufferPush(shift ? letters[i].uppercase : letters[i].lowercase);
        break;
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
    for(i = 0 ; i<100000 ; i++)
      ;
  }
  return c;
}

