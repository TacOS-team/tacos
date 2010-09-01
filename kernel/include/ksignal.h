#ifndef _KSIGNAL_H
#define _KSIGNAL_H

#include <ksyscall.h>
#include <signal.h>
#include <types.h>
#include <process.h>

SYSCALL_HANDLER3(sys_signal, uint32_t param1, uint32_t param2, uint32_t param3);
SYSCALL_HANDLER3(sys_sigprocmask, uint32_t param1, uint32_t param2, uint32_t param3);
SYSCALL_HANDLER3(sys_kill, int pid, int signum, int* ret);

int exec_sighandler(process_t* process);

#endif /* _KSIGNAL_H */
