/**
 * @file signal.c
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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

#include <signal.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

static sighandler_t default_handlers[NSIG];

int kill(unsigned int pid, int sig)
{
	int ret;
	
	syscall(SYS_KILL, (uint32_t) pid, (uint32_t) sig, (uint32_t) &ret);
	
	return ret;
}


sighandler_t signal(int sig, sighandler_t func)
{
	sighandler_t ret;
	
	if (func == SIG_DFL) {
		func = default_handlers[sig];
	}

	syscall(SYS_SIGNAL, (uint32_t)sig, (uint32_t)func, (uint32_t)&ret);
	
	return ret;
}

int sigsuspend(const sigset_t *sigmask) {
	syscall(SYS_SIGSUSPEND, (uint32_t)sigmask, (uint32_t)NULL, (uint32_t)NULL);
	//XXX TODO errno
	return -1;
}


int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		syscall(SYS_SIGPROCMASK, (uint32_t)how, (uint32_t)set, (uint32_t)oldset);
	}
	return ret;
}


int sigemptyset(sigset_t *set)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set = 0;
	}
	return ret;
}

int sigfillset(sigset_t *set)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set = 0xFFFFFFFF;
	}
	return ret;
}

int sigaddset(sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set |= (1<<signum);
	}
	return ret;
}

int sigdelset(sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
	{
		ret = 0;
		*set &= ~(1<<signum);
	}
	return ret;
}

int sigismember(const sigset_t *set, int signum)
{
	int ret = -1;
	if(set != NULL)
		ret = 0x1 & (*set>>signum);
	
	return ret;
}

void sig_ignore_handler(int signal __attribute__ ((unused)))
{
	// Do nothing.
}

void core(int signal)
{
	exit(signal);
}

void sig_stop_handler(int signal __attribute__ ((unused)))
{
	sigset_t set;
	sigfillset(&set);
	sigdelset(&set, SIGCONT);
	sigdelset(&set, SIGINT);
	sigdelset(&set, SIGKILL);
	sigsuspend(&set);
}

void init_signals(void)
{
	default_handlers[SIGHUP] = (sighandler_t) exit;
	default_handlers[SIGINT] = (sighandler_t) exit;
	default_handlers[SIGQUIT] = core;
	default_handlers[SIGILL] = core;
	default_handlers[SIGTRAP] = core;
	default_handlers[SIGABRT] = core;
	default_handlers[SIGFPE] = core;
	default_handlers[SIGSEGV] = core;
	default_handlers[SIGSYS] = core;
	default_handlers[SIGKILL] = (sighandler_t) exit;
	default_handlers[SIGPIPE] = (sighandler_t) exit;
	default_handlers[SIGALRM] = (sighandler_t) exit;
	default_handlers[SIGTERM] = (sighandler_t) exit;
	default_handlers[SIGUSR1] = (sighandler_t) exit;
	default_handlers[SIGUSR2] = (sighandler_t) exit;
	default_handlers[SIGCHLD] = sig_ignore_handler;
	default_handlers[SIGPWR] = sig_ignore_handler;
	default_handlers[SIGWINCH] = sig_ignore_handler;
	default_handlers[SIGURG] = sig_ignore_handler;
	default_handlers[SIGPOLL] = (sighandler_t) exit;
	default_handlers[SIGSTOP] = (sighandler_t) sig_stop_handler;
	default_handlers[SIGTSTP] = (sighandler_t) sig_stop_handler;
	default_handlers[SIGCONT] = sig_ignore_handler;
	default_handlers[SIGTTIN] = (sighandler_t) sig_stop_handler;
	default_handlers[SIGTTOU] = (sighandler_t) sig_stop_handler;
	default_handlers[SIGVTALRM] = (sighandler_t) exit;
	default_handlers[SIGPROF] = (sighandler_t) exit;
	default_handlers[SIGRTMIN] = (sighandler_t) exit;
	default_handlers[SIGRTMAX] = (sighandler_t) exit;

	int i;
	for (i = 0; i < NSIG; i++) {
		signal(i, SIG_DFL);
	}
}
