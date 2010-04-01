#include "stdio.h"

/**
 * Affiche un message sur la sortie standard stdout.
 *
 * @return En cas de succès, ces fonctions renvoient le nombre de caractères
 * affichés. Négatif en cas d'erreur.
 */
int printf(const char *format, ...) {
    va_list arg;
    int result;
 
    va_start(arg, format);
    result = vfprintf(stdout, format, arg);
    va_end(arg);
 
    return result;
}

int vprintf(const char *format, va_list ap) {
	return vfprintf(stdout, format, ap);
}
