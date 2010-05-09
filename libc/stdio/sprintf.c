#include "stdio.h"

/**
 * Affiche un message dans la chaine str.
 *
 * @return En cas de succès, cette fonction renvoie le nombre de caractères
 * affichés. Négatif en cas d'erreur.
 */
int sprintf(char *str, const char *format, ...) {
    va_list arg;
    int result;
 
    va_start(arg, format);
    result = vsprintf(str, format, arg);
    va_end(arg);
 
    return result;
}

/**
 * Affiche un message dans la chaine str sans dépasser size caractères.
 *
 * @return En cas de succès, cette fonction renvoie le nombre de caractères 
 * affichés. Négatif en cas d'erreur.
 */
int snprintf(char *str, size_t size, const char *format, ...) {
    va_list arg;
    int result;
 
    va_start(arg, format);
    result = vsnprintf(str, size, format, arg);
    va_end(arg);
 
    return result;
}

int vsprintf(char *str, const char *format, va_list ap) {
	FILE * stream = fmemopen(str, 10000, "r");
	return vfprintf(stream, format, ap);
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap) {
	if (size > 0) {
		str[size - 1] = '\0';
		FILE * stream = fmemopen(str, size, "r");
		return vfprintf(stream, format, ap);
	}
	return 0;
}


