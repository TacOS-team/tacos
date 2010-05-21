/**
 * @file sprintf.c
 */

#include "stdio.h"

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
