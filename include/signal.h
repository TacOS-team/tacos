#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <types.h>	

#define NSIG		32
typedef unsigned long sigset_t;
typedef void (*sighandler_t)(int);

typedef struct {
	void *ss_sp;
	int ss_flag;
	size_t ss_size;
} sstack_t;

typedef struct
{
	sigset_t mask;
	sigset_t pending_set;
	sighandler_t handlers[NSIG];
	sstack_t stack;
}signal_process_data_t;

/* Définition des signaux */
#define SIGHUP		 0
#define SIGINT		 1
#define SIGQUIT		 2
#define SIGILL		 3
#define SIGTRAP		 4
#define SIGABRT		 5
#define SIGIOT		 5
#define SIGBUS		 6
#define SIGFPE		 7
#define SIGKILL		 8
#define SIGUSR1		 9	
#define SIGSEGV		10
#define SIGUSR2		11
#define SIGPIPE		12
#define SIGALRM		13
#define SIGTERM		14
#define SIGSTKFLT	15
#define SIGCHLD		16
#define SIGCONT		17
#define SIGSTOP		18
#define SIGTSTP		19
#define SIGTTIN		20
#define SIGTTOU		21
#define SIGURG		22
#define SIGXCPU		23
#define SIGXFSZ		24
#define SIGVTALRM	25
#define SIGPROF		26
#define SIGWINCH	27
#define SIGIO		28
#define SIGPOLL		SIGIO
#define SIGPWR		29
#define SIGSYS		30
#define	SIGUNUSED	30
#define SIGRTMIN	31
#define SIGRTMAX	NSIG-1

/* Paramètre pour sigprocmask */
#define SIG_BLOCK	0
#define SIG_UNBLOCK	1
#define SIG_SETMASK	2

int kill(unsigned int pid, int sig);

sighandler_t signal(int sig, sighandler_t func);

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

int sigemptyset(sigset_t *set);

int sigfillset(sigset_t *set);

int sigaddset(sigset_t *set, int signum);

int sigdelset(sigset_t *set, int signum);

int sigismember(const sigset_t *set, int signum);


#endif /* _SIGNAL_H */
