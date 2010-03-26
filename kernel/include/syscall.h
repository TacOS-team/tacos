#ifndef _SYSCALL_H_
#define _SYSCALL_H_

void init_syscall();
void syscall_update_esp(paddr_t esp);
void syscall(int func);

#endif
