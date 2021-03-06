/**
 * @file syscall_values.h
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
 * Liste des SYSCALL de TacOS.
 */

#ifndef _SYSCALL_VALUES_H_
#define _SYSCALL_VALUES_H_

#define SYS_EXIT 0				/**< Terminer le processus. */
#define SYS_GETPID 1			/**< Obtient le PID du processus. */
#define SYS_GETPPID 2 		/**< Obtient le PPID du processus. */
#define SYS_OPEN 3				/**< Ouverture d'un fichier. */
#define SYS_KILL 4				/**< Kill sur un processus. */
#define SYS_WRITE 5				/**< Ecriture de fichier. */
#define SYS_READ 6				/**< Lecture de fichier. */
#define SYS_EXEC 7				/**< Exécution d'un processus. */
#define SYS_SLEEP 8				/**< Mise en pause. */
#define SYS_SEMCTL 9			/**< Configuration d'un sémaphore. */
#define SYS_VMM 12				/**< Allocation mémoire. */
#define SYS_SEEK 13				/**< Déplacement dans un fichier ouvert. */
#define SYS_READLINK 14			/**< Lecture chemin pointé par lien symbolique. */
#define SYS_SIGNAL 15			/**< Envoi de signal. */
#define SYS_SIGPROCMASK 16/**< Configuration masque signal. */
#define SYS_CLOSE 17			/**< Fermeture d'un fichier ouvert. */
#define SYS_MKNOD 18			/**< Création d'un noeud. */
#define SYS_READDIR 19		/**< Lecture de dossier. */
#define SYS_MKDIR 20			/**< Création d'un dossier vide. */
#define SYS_SIGRET 21			/**< Syscall appelé au retour d'un handler de signal.*/
#define SYS_SIGSUSPEND 22 /**< Attend un signal. */ 
#define SYS_IOCTL 23			/**< Control périphérique. */
#define SYS_GETCLOCK 24		/**< Obtient l'heure. */
#define SYS_GETDATE 25		/**< Obtient la date. */
#define SYS_FCNTL 26			/**< Manipulation de fichier. */
#define SYS_DUMMY 27			/**< Dummy syscall. */
#define SYS_STAT 28				/**< Infos d'un fichier. */
#define SYS_UNLINK 29			/**< Suppression d'un fichier ou dossier. */
#define SYS_DUP 30				/**< Duplique un descripteur de fichier. */
#define SYS_WAITPID 31		/**< Attend la fin d'un processus. */
#define SYS_RMDIR 32			/**< Suppression d'un dossier vide. */
#define SYS_DUP2 33				/**< Duplique un descripteur de fichier en précisant le nouveau fd à utiliser. */
#define SYS_CHMOD 34			/**< Change les droits d'un fichier. */
#define SYS_CHOWN 35			/**< Change les propriétaires d'un fichier. */
#define SYS_UTIMES 36			/**< Change la date d'accès et de modification d'un fichier. */
#define SYS_RENAME 37			/**< Déplace ou renomme un fichier. */
#define SYS_SELECT 38			/**< Utile à la commande select() pour le multiplexage d'io. */
#define SYS_SYMLINK 39			/**< Création d'un lien symbolique. */
#define SYS_CHDIR 40			/**< Change le dossier courant. */
#define SYS_MMAP 41				/**< Permet d'établir/supprimer un mapping mémoire d'un fichier. */

#endif
