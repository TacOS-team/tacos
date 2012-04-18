/**
 * @file unistd.h
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

#ifndef _UNISTD_H_
#define _UNISTD_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * @file unistd.h
 */

#include <sys/types.h>
#include <sys/stat.h>

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 1

/**
 * @brief Change le current working directory.
 *
 * Change le current working directory.
 *
 * @param path Le chemin où on veut aller.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int chdir(const char *path);


/**
 * @brief Retourne l'actuel working directory.
 *
 * Retourne l'actuel working directory.
 *
 * @param buf Table qui prend le chemin actuel. Si NULL, alors un malloc 
 * est fait.
 * @param size Taille du buffer. Si le buffer est trop petit, une erreur 
 * sera renvoyée.
 *
 * @return Le chemin actuel. NULL en cas d'erreur.
 */
const char * getcwd(char * buf, size_t size);

/**
 * @brief Retourne le chemin absolu d'un path en fonction du cwd.
 *
 * Retourne le chemin absolue d'un path en fonction du cwd à partir 
 * d'un chemin relatif.
 *
 * @param dirname Chemin relatif.
 *
 * @return Chemin absolu.
 */
char * get_absolute_path(const char *dirname);

/**
 * @brief Obtenir l'identifiant du processus.
 *
 * Obtenir l'identifiant du processus.
 *
 * @return L'identifiant du processus.
 */
pid_t getpid(void);

/**
 * @brief Obtenir l'identifiant du processus père.
 *
 * Obtenir l'identifiant du processus père.
 *
 * @return L'identifiant du processus père.
 */
pid_t getppid(void);

/**
 * @brief Réalise l'appel-système indiqué par son identifiant.
 *
 * Réalise l'appel-système indiqué par son identifiant.
 *
 * @param func L'identifiant du syscall.
 */
void syscall(uint32_t func, uint32_t param1, uint32_t param2, uint32_t param3);

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
 * @param microseconds Le nombre de microsecondes pendant lesquelles le 
 * processus doit être endormi.
 * 
 * @return 0 en cas de succès, -1 en cas d'erreur.
 */
unsigned int usleep(unsigned int microseconds);

/**
 * @brief Écrire dans un descripteur de fichier.
 *
 * Lit au maximum count octets dans la zone mémoire pointée par
 * buf, et les écrit dans le fichier référencé par le descripteur fd.
 *
 * @param fd Numero du descripteur de fichier ouvert.
 * @param buf Buffer qui contient les données à écrire.
 * @param count Le nombre maximum d'octets à écrire.
 *
 * @return le nombre d'octets écrits (0 si aucune écriture, -1 en cas d'erreur).
 */
ssize_t write(int fd, const void *buf, size_t count);

/**
 * @brief Lire depuis un descripteur de fichier.
 *
 * Lit jusqu'à count octets depuis le descripteur de fichier fd dans le tampon
 * pointé par buf.
 *
 * @param fd Numero du descripteur de fichier ouvert.
 * @param buf Buffer où enregistrer les données lues.
 * @param count Nombre maximum d'octets à lire.
 *
 * @return le nombre d'octets lus (0 en fin de fichier, -1 en cas d'erreur).
 */
ssize_t read(int fd, void *buf, size_t count);

/**
 * @brief Positionner la tête de lecture/écriture dans un fichier.
 *
 * Place la tête de lecture/écriture à la position offset dans le fichier
 * associé au descripteur fd en suivant la directive whence.
 *
 * @param fd Numero du descripteur de fichier ouvert.
 * @param offset Position dans le fichier.
 * @param whence Directive (SET, CUR, END)
 *
 * @return offset depuis le début du fichier. -1 en cas d'erreur.
 */
int lseek(int fd, long offset, int whence);

/**
 * @brief Obtenir l'état d'un fichier (status).
 *
 * Renvoie des informations à propos du fichier indiqué.
 *
 * @param path Chemin du fichier.
 * @param buf Structure qui va recevoir les informations.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int stat(const char *path, struct stat *buf);

/**
 * @brief Détruire un nom et éventuellement le fichier associé.
 *
 * Détruit un nom dans le système de fichier. Actuellement, le fichier associé
 * est toujours effacé, même s'il y a plusieurs liens. (TODO améliorer ça)
 *
 * @param pathname Chemin du fichier.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int unlink(const char *pathname);

/**
 * @brief Supprimer un dossier, lequel doit être vide.
 *
 * @param pathname Chemin du dossier.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int rmdir(const char *pathname);

/**
 * @brief Créer un noeud du système de fichiers.
 *
 * Crée un noeud du système de fichier (fichier, fichier spécial, etc.)
 *
 * @param path Chemin du noeud.
 * @param mode Droit d'accès sur ce noeud.
 * @param dev Dans le cas d'un fichier de périphérique, c'est son identifiant.
 *
 * @return 0 en cas de succès, -1 sinon.
 */
int mknod(const char *path, mode_t mode, dev_t dev);

__END_DECLS

#endif //_UNISTD_H_
