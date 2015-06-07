/**
 * @file signal.h
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010-2015 TacOS developers.
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
 * @brief Gestion des signaux.
 */

#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <sys/types.h>	

#define NSIG		32 /**< Nombre de signaux. */
#ifndef SIGTYPES
#define SIGTYPES
typedef unsigned long sigset_t;
typedef void (*sighandler_t)(int); /**< Type pour un handler de signal. */
#endif

/* Définition des signaux */
//XXX: Est-ce qu'on ne devrait pas se caller sur les numéros POSIX ?
#define SIGHUP		 0 /**< Hang up. */
#define SIGINT		 1 /**< Interrupt. */
#define SIGQUIT		 2 /**< Quit and dump. */
#define SIGILL		 3 /**< Illegal instruction. */
#define SIGTRAP		 4 /**< Trap. */
#define SIGABRT		 5 /**< Abort. */
#define SIGBUS		 6 /**< Bus error. */
#define SIGFPE		 7 /**< Floating Point Exception. */
#define SIGKILL		 8 /**< Terminate. */
#define SIGUSR1		 9 /**< Signal utilisateur 1. */
#define SIGSEGV		10 /**< Segmentation Violation. */
#define SIGUSR2		11 /**< Signal utilisateur 2. */
#define SIGPIPE		12 /**< Write to pipe with no one reading. */
#define SIGALRM		13 /**< Signal raised by alarm. */
#define SIGTERM		14 /**< Termination. */
#define SIGCHLD		16 /**< Fin processus fils. */
#define SIGCONT		17 /**< Continue si arrêté. */
#define SIGSTOP		18 /**< Stop l'exécution temporairement. */
#define SIGTSTP		19 /**< Terminal Stop. */
#define SIGTTIN		20 /**< Le process en background essaye de lire le tty. */
#define SIGTTOU		21 /**< Le process en background essaye d'écrire sur le tty. */
#define SIGURG		22 /**< Urgent data available on socket. */
#define SIGSYS		30 /**< Bad syscall. */
#define SIGRTMIN	31
#define SIGRTMAX	NSIG-1

/* Paramètre pour sigprocmask */
#define SIG_BLOCK	0
#define SIG_UNBLOCK	1
#define SIG_SETMASK	2

#define SIG_IGN     sig_ignore_handler  /**< Ignorer le signal. */
#define SIG_DFL			0										/**< Utiliser le handler par défaut. */

/**
 * Mise en place des handlers par défaut.
 */
void init_signals(void);

/**
 * Envoyer un signal à un processus.
 *
 * @param pid l'identifiant du processus.
 * @param sig le signal à envoyer.
 *
 * @return 0 en cas de succès.
 */
int kill(unsigned int pid, int sig);

/**
 * Envoyer un signal à l'appelant.
 *
 * @param sig le signal à envoyer.
 *
 * @return 0 en cas de succès.
 */
int raise(int sig);

/**
 * Met en place un handler pour un signal.
 *
 * @param sig le signal à catcher.
 * @param func fonction handler à utiliser.
 *
 */
sighandler_t signal(int sig, sighandler_t func);

int sigsuspend(const sigset_t *sigmask);

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

int sigemptyset(sigset_t *set);

int sigfillset(sigset_t *set);

int sigaddset(sigset_t *set, int signum);

int sigdelset(sigset_t *set, int signum);

int sigismember(const sigset_t *set, int signum);

void sig_ignore_handler(int signal);

__END_DECLS

#endif /* _SIGNAL_H */
