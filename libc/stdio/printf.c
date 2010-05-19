/**
 * @file printf.c
 */

#include <stdio.h>

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
