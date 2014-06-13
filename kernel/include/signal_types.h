/**
 * @file signal_types.h
 *
 * @author TacOS developers 
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
 * @brief Types pour les signaux (en particulier les numéros de signaux).
 */

#ifndef _SIGNAL_TYPES_H
#define _SIGNAL_TYPES_H

#define NSIG		32

#ifndef SIGTYPES
#define SIGTYPES
typedef unsigned long sigset_t;
typedef void (*sighandler_t)(int);
#endif

typedef struct
{
	sigset_t mask;
	sigset_t pending_set;
	sighandler_t handlers[NSIG];
} signal_process_data_t;

/* Définition des signaux (/!\ code aussi dupliqué dans la libc) */
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
// On a perdu le 15?
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
// Penser à modifier la liste des noms de signaux dans ksignal.c en cas de modification.

/* Paramètre pour sigprocmask */
#define SIG_BLOCK	0
#define SIG_UNBLOCK	1
#define SIG_SETMASK	2


#endif
