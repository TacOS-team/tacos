/**
 * @file unistd.h
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

#ifndef _UNISTD_H_
#define _UNISTD_H_

/**
 * @file unistd.h
 */

#include <types.h>
#include <sys/types.h>

//TODO(max) Commenter ces fonctions.
int chdir(const char *path);
const char * getcwd(char * buf, size_t size);
char * get_absolute_path(const char *dirname);
pid_t getpid(void);

/** 
 * @brief Endort le processus pour une durée déterminée (en secondes).
 *
 * Endort le processus pour une durée déterminée (en secondes).
 * 
 * @see usleep
 *
 * @param seconds Le nombre de secondes pendant lesquelles le processus
 * doit être endormi.
 * 
 * @return 0 si le temps prévu s'est écoulé.
 */

unsigned int sleep(unsigned int seconds);

/** 
 * @brief Endort le processus pour une durée déterminée (en microsecondes).
 * 
 * Endort le processus pour une durée déterminée (en microsecondes).
 *
 * @param milliseconds Le nombre de microsecondes pendant lesquelles le 
 * processus doit être endormi.
 * 
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
unsigned int usleep(unsigned int microseconds);

ssize_t write(int fd, const void *buf, size_t count);

ssize_t read(int fd, void *buf, size_t count);

int seek(int fd, long offset, int whence);

#endif //_UNISTD_H_
