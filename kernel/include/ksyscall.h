#ifndef _KSYSCALL_H_
#define _KSYSCALL_H_

#include <types.h>

#define MAX_SYSCALL_NB 256

typedef void (*syscall_handler_t)(uint32_t,uint32_t,uint32_t);

void init_syscall();
int syscall_set_handler(uint32_t syscall_id, syscall_handler_t handler);

#endif
