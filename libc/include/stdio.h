/**
 * @file stdio.h
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
 * Description de ce que fait le fichier
 */

#ifndef _STDIO_H_
#define _STDIO_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <libio.h>
#include <sys/types.h>
#include <stdarg.h>

/* End Of File character. */
#ifndef EOF
# define EOF (-1)
#endif

#define _IOFBF 0 /* Fully buffered. */
#define _IOLBF 1 /* Line buffered. */
#define _IONBF 2 /* No buffering. */

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef long fpos_t;

/**
 * Liste chainée qui contient les streams. 
 */
extern FILE* __file_list; 

/**
 * @brief Affiche un message sur la sortie standard stdout.
 *
 * Affiche un message sur la sortie standard stdout (peut donc être buffurisé et ce n'est pas nécessairement l'écran).
 *
 *	@see fprintf
 *	@see sprintf
 *	@see snprintf
 *	@see vprintf
 *	@see vfprintf
 *	@see vsprintf
 *	@see vsnprintf
 *
 * @param format chaîne de format précisant le format de conversion pour la
 * sortie.
 * @param ... liste variable d'arguments à afficher.
 *
 * @return En cas de succès, renvoit le nombre de caractères affichés.
 * Négatif en cas d'erreur.
 */
int printf(const char *format, ...);

/** 
 * @brief Affiche un message dans le stream passé en argument.
 * 
 * Affiche un message dans le stream passé en argument. En effectuant les conversions.
 *
 *	@see fprintf
 *	@see sprintf
 *	@see snprintf
 *	@see vprintf
 *	@see vfprintf
 *	@see vsprintf
 *	@see vsnprintf
 *
 * @param stream un pointeur sur un stream.
 * @param format chaîne de format précisant le format de conversion pour la
 * sortie.
 * @param ... liste variable d'arguments à afficher.
 * 
 * @return En cas de succès, renvoit le nombre de caractères affichés.
 * Négatif en cas d'erreur.
 */
int fprintf(FILE *stream, const char *format, ...);

/** 
 * @brief Écrit un message dans le buffer passé en argument.
 *
 * Écrit un message (en utilisant la syntaxe printf habituelle) dans le buffer
 * passé en argument. Attention, aucune allocation mémoire n'est faite.
 *
 *	@see fprintf
 *	@see sprintf
 *	@see snprintf
 *	@see vprintf
 *	@see vfprintf
 *	@see vsprintf
 *	@see vsnprintf
 * 
 * @param str le buffer dans lequel on écrit.
 * @param format chaîne de format précisant le format de conversion pour la 
 * sortie.
 * @param ... liste variable d'arguments à afficher.
 * 
 * @return En cas de succès, renvoit le nombre de caractères affichés.
 * Négatif en cas d'erreur.
 */
int sprintf(char *str, const char *format, ...);

/** 
 * @brief Écrit un message dans le buffer passé en argument avec une limite 
 * sur le nombre de caractères à écrire.
 *
 * Écrit un message (en utilisant la syntaxe printf habituelle) dans le buffer
 * passé en argument. Le nombre de caractères est limité. Attention, aucune 
 * allocation mémoire n'est faite.
 * Attention !!! Cette limitation n'est pas encore implémentée ! Seul un 
 * caractère nul sera correctement placé au n-ième caractère.
 *
 *	@see printf
 *	@see fprintf
 *	@see snprintf
 *	@see vprintf
 *	@see vfprintf
 *	@see vsprintf
 *	@see vsnprintf
 * 
 * @param str le buffer dans lequel on écrit.
 * @param size le nombre maximum de caractères à écrire dans le buffer.
 * @param format chaîne de format précisant le format de conversion pour
 * la sortie.
 * @param ... liste variable d'arguments à afficher.
 * 
 * @return En cas de succès, renvoit le nombre de caractères affichés.
 * Négatif en cas d'erreur.
 */
int snprintf(char *str, size_t size, const char *format, ...);

int vprintf(const char *format, va_list ap);
int vfprintf(FILE *stream, const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);

int scanf(const char *format, ...);
int sscanf(const char *s, const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);

int vsscanf(const char *s, const char *format, va_list ap);
int vfscanf(FILE *stream, const char *format, va_list ap);
int vscanf(const char *format, va_list ap);

int fputc(int c, FILE *stream);
int fputs(const char *s, FILE *stream);
int putc(int c, FILE *stream);
int putchar(int c);
int puts(const char *s);
int fgetline(FILE *fp, char s[], int lim);
int getline(char *s, int lim);

/**
 * @brief Écriture binaire sur un flux.
 *
 * Cette fonction écrit jusqu'à nmemb elements dont la taille est 
 * définie par size vers le stream pointé par le pointeur stream.
 * Pour chaque elements, fwrite fait size appels à fputc en castant 
 * simplement l'element en un tableau de uchar.
 *
 * @param ptr Pointeur vers les données à écrire.
 * @param size Taille d'un élément de données.
 * @param nmemb Nombre d'éléments.
 * @param stream Stream à utiliser.
 *
 * @return Nombre d'éléments écrits.
 */
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

/** 
  * @brief Lit au plus size-1 caractères depuis le stream passé en argument.
  *
  * Lit au plus size-1 caractères depuis le stream passé en argument. La 
  * lecture s'arrête par EOF, retour chariot ou si size-1 caractères ont 
  * été lus.
  * Attention, il n'y a pas d'allocation mémoire pour le buffer où on écrit.
  * Le caractère nul est placé à la fin de la chaîne.
  *
  * @param s buffer où on va écrire ce qu'on lit sur stream.
  * @param size la taille du buffer.
  * @param stream le stream depuis lequel on lit les caractères.
  * 
  * @return un pointeur sur s en cas de succès. NULL en cas d'erreur.
  */
char *fgets(char *s, int size, FILE *stream);

/**
 * @brief Lit un caractère depuis le flux stream.
 *
 * Fgetc lit le caractère suivant depuis le flux stream.
 *
 * @parame stream le stream depuis lequel on lit le caractère.
 *
 * @return le caractère lu ou EOF en cas d'erreur ou fin de flux.
 */
int fgetc(FILE *stream);

#define getc(stream) fgetc(stream)

/** 
 * @brief lit un caractère depuis stdin.
 *
 * Lit un caractère depuis stdin. Strictement équivalent à fgetc(stdin).
 * 
 * @return le caractère lu ou EOF en cas d'erreur ou fin de flux.
 */
int getchar(void);

int fflush(FILE *stream);

int fclose(FILE* fd);
int fcloseall(void);

FILE *fopen(const char *path, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);

/**
 *	Cette fonction associe un buffer de taille size à un nouveau stream.
 *	Les modes sont les même que fopen.
 *
 *	@return le nouveau stream.
 */
FILE *fmemopen(void *buf, size_t size, const char *mode);


/**
 * @brief Modifie le buffer et les flags d'un stream
 * 
 * Modifie le buffer et les flags d'un stream
 * 
 * @param stream le stream a modifier
 * @param buf le nouveau buffer (peut être défini à NULL et sera alloué
 *  automatiquement.
 * @param mode les nouveaux flags
 * @param size taille du nouveau buffer
 *
 * @return 0 si tout s'est bien passé.
 */
int setvbuf(FILE *stream, char *buf, int mode, size_t size);

/**
 * @brief Initialise les streams de base.
 */
void init_stdfiles(void);

/**
 * @brief Fixe l'indicateur de position du flux.
 *
 * @param stream Le stream considéré.
 * @param offset Le décalage.
 * @param whence Méthode : depuis le début, la fin ou en relatif.
 *
 * @return 0 si tout s'est bien passé.
 */
int fseek(FILE *stream, long offset, int whence);

/**
 * @brief Obtient la valeur de l'indicateur de position.
 *
 * @param stream Le stream considéré.
 *
 * @return la position absolue.
 */
long ftell(FILE *stream);

/**
 * @brief Place l'indicateur de position au début du fichier.
 *
 * @param stream Le stream considéré.
 *
 * @return 0 si tout s'est bien passé.
 */
void rewind(FILE *stream);

int fgetpos(FILE *stream, fpos_t *pos);
int fsetpos(FILE *stream, fpos_t *pos);

/**
 * @brief Supprimer un fichier ou répertoire.
 *
 * Supprime un fichier en utilisant unlink et un dossier avec rmdir.
 *
 * @param pathname Chemin du fichier à supprimer.
 *
 * @return ce que return unlink et rmdir.
 */
int remove(const char *pathname);

/**
 * @brief Efface la fin de fichier et les indicateurs d'erreur du flux.
 *
 * @param stream Flux à réinitialiser.
 */
void clearerr(FILE *stream);

/**
 * @brief Teste l'indicateur de fin de fichier.
 *
 * @param stream Flux à tester.
 *
 * @return 0 si faux, sinon vrai.
 */
int feof(FILE *stream);

/**
 * @brief Teste l'indicateur d'erreur du flux.
 *
 * @param stream Flux à tester.
 *
 * @return non nul si indicateur actif.
 */
int ferror(FILE *stream);

/**
 * @brief Renvoie le numéro de descripteur de fichier associé au stream.
 *
 * @param stream Flux à analyser.
 *
 * @return le numéro du descripteur de fichier.
 */
int fileno(FILE *stream);

/**
 * @brief Affiche un message d'erreur système.
 *
 * Afficher le message donné en argument suivi du message associé à l'erreur 
 * (errno).
 *
 * @param s le message à afficher avant l'erreur.
 */
void perror(const char *s);

__END_DECLS

#endif
