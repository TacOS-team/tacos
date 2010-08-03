#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned char uint8_t;  /* 8 bits non signé */
typedef unsigned short int uint16_t;  /* 16 bits non signé */
typedef unsigned long int uint32_t;  /* 32 bits non signé */
typedef unsigned long long int uint64_t;  /* 64 bits non signé */

typedef uint32_t size_t;
typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

typedef int ssize_t;

typedef enum {FALSE=0, TRUE} bool;

/* Pointeur null */
#define NULL ((void*)0)

#endif
