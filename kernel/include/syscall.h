#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <types.h>

void init_syscall();
void syscall_update_esp(paddr_t esp);
void syscall(uint32_t func, uint32_t param1, uint32_t param2, uint32_t param3)

#endif
