#include <libtacos.h>
#include <stdio.h>
#include <sys/types.h>

void debug(const char *format, ...) {
//#ifdef DEBUG
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
//#endif
}

void syscall(uint32_t func, uint32_t param1, uint32_t param2, uint32_t param3)
{
  // On met la fonction et les parametres dans les bon registres
  asm(""::"a"(func),"b"(param1),"c"(param2),"d"(param3));
  
  // et on lance l'interruption
  asm("int $0x30");
}
