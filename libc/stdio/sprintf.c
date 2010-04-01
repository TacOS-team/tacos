#include "stdio.h"

/**
 * Affiche un message dans la chaine str.
 *
 * @return En cas de succès, ces fonctions renvoient le nombre de caractères
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

int snprintf(char *str, size_t size, const char *format, ...) {
    va_list arg;
    int result;
 
    va_start(arg, format);
    result = vsnprintf(str, size, format, arg);
    va_end(arg);
 
    return result;
}

int vsprintf(char *str, const char *format, va_list ap) {
	FILE stream;
	stream._fileno = -1;
	stream._IO_buf_base = str;
	stream._IO_buf_end = str + 10000; // Normalement on dépasse pas, sinon c'est la faute au dev.
	stream._IO_write_base = str;
	stream._IO_write_ptr = str;
	stream._IO_write_end = str;
	stream._flags = _IO_MAGIC + _IO_UNBUFFERED;
	return vfprintf(&stream, format, ap);
}

int vsnprintf(char *str, size_t size, const char *format, va_list ap) {
	if (size > 0) {
		FILE stream;
		stream._fileno = -1;
		stream._IO_buf_base = str;
		stream._IO_buf_end = str + size; 
		stream._IO_write_base = str;
		stream._IO_write_ptr = str;
		stream._IO_write_end = str;
		stream._flags = _IO_MAGIC + _IO_UNBUFFERED;
		str[size - 1] = '\0';
		return vfprintf(&stream, format, ap);
	}
	return 0;
}


