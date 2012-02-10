/**
 * @file signal.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <sys/types.h>	

#define NSIG		32
#ifndef SIGTYPES
#define SIGTYPES
typedef unsigned long sigset_t;
typedef void (*sighandler_t)(int);
#endif

/* Définition des signaux */
//XXX: Est-ce qu'on ne devrait pas se caller sur les numéros POSIX ?
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

#define SIG_IGN     sig_ignore_handler

int kill(unsigned int pid, int sig);

sighandler_t signal(int sig, sighandler_t func);

int sigsuspend(const sigset_t *sigmask);

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

int sigemptyset(sigset_t *set);

int sigfillset(sigset_t *set);

int sigaddset(sigset_t *set, int signum);

int sigdelset(sigset_t *set, int signum);

int sigismember(const sigset_t *set, int signum);

void init_signals(void);


void sig_ignore_handler(int signal);

#endif /* _SIGNAL_H */
