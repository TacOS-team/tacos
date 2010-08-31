#ifndef _KSIGNAL_H
#define _KSIGNAL_H

#include <signal.h>
#include <types.h>

void sys_signal(uint32_t param1, uint32_t param2, uint32_t param3);
void sys_sigprocmask(uint32_t param1, uint32_t param2, uint32_t param3);
void sys_kill(int pid, int signum, int* ret);

#endif /* _KSIGNAL_H */
