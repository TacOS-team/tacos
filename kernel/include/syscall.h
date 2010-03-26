#ifndef _SYSCALL_H_
#define _SYSCALL_H_

#include <types.h>

#define MAX_SYSCALL_NB 256

typedef void (*syscall_handler_t)(uint32_t,uint32_t,uint32_t);

void init_syscall();
int syscall_set_handler(uint32_t syscall_id, syscall_handler_t handler);
void syscall(uint32_t func, uint32_t param1, uint32_t param2, uint32_t param3);

#endif
