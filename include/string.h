#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

void* memcpy(void* dest, const void* src, size_t size);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, size_t n);

#endif
