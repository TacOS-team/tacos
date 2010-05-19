#ifndef _STDIO_H_
#define _STDIO_H_

/**
 * @file stdio.h
 */

#include <fcntl.h>
#include <types.h>
#include <stdarg.h>

/* End Of File character. */
#ifndef EOF
# define EOF (-1)
#endif

#define _IOFBF 0 /* Fully buffered. */
#define _IOLBF 1 /* Line buffered. */
#define _IONBF 2 /* No buffering. */

size_t write_screen(open_file_descriptor *ofd, const void *buf, size_t count);

void kprintf(const char *format, ...);

/**
 * @brief Affiche un message sur la sortie standard stdout.
 *
 * @param format chaîne de format précisant le format de conversion pour la
 * sortie.
 * @param ... liste variable d'arguments.
 *
 * @return En cas de succès, renvoit le nombre de caractères affichés.
 * Négatif en cas d'erreur.
 */
int printf(const char *format, ...);

/** 
 * @brief Affiche un message dans le stream passé en argument.
 * 
 * @param stream un pointeur sur un stream.
 * @param format chaîne de format précisant le format de conversion pour la
 * sortie.
 * @param ... liste variable d'arguments.
 * 
 * @return En cas de succès, renvoit le nombre de caractères affichés.
 * Négatif en cas d'erreur.
 */
int fprintf(FILE *stream, const char *format, ...);
int sprintf(char *str, const char *format, ...);
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

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

char *fgets(char *s, int size, FILE *stream);
int fgetc(FILE *stream);
int getchar(void);

int fflush(FILE *stream);

FILE *fopen(const char *path, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *path, const char *mode, FILE *stream);

FILE *fmemopen(void *buf, size_t size, const char *mode);

/** 
* @brief Initialise de nouveaux stdin, stdout et stderr.
* 
* @param _stdin pointeur sur un stdin à initialiser.
* @param _stdout pointeur sur un stdout à initialiser.
* @param _stderr pointeur sur un stderr à initialiser.
*/
void init_stdfiles(FILE ** _stdin, FILE ** _stdout, FILE ** _stderr);


/** 
* @brief Converti l'entier d en une chaîne de caractère et le stock dans buf.
*	 L'entier base permet de spécifier la base à utiliser (decimal ou 
*	 hexadécimal).
*
* Converti l'entier d en une chaîne de caractère et le stock dans buf. Si 
* base est égal à 'd', alors il interprète d comme étant en décimal et si 
* base est égal à 'x', alors il interprète d comme étant en hexadécimal.
* 
* @param buf une chaîne de taille suffisament grande pour y stocker le 
*	 résultat de la transformation.
* @param base la base à utiliser pour la conversion ('d' ou 'x').
* @param d le nombre à convertir.
*/
void itoa (char *buf, int base, int d);


#endif
