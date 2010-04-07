#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <types.h>

#define SYS_EXIT 0
#define SYS_GETPID 1
#define SYS_OPEN 3
#define SYS_KILL 4

void syscall(uint32_t func, uint32_t param1, uint32_t param2, uint32_t param3);

#endif
