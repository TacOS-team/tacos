#include <types.h>
#include <stdio.h>
#include <ioports.h>
#include <keyboard.h>

void keyboardInterrupt(int id)
{
  uint8_t code = inb(0x60);
  printf("Key pressed %d -> %c\n", id, code);
}

