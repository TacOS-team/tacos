#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

typedef int pid_t;

#ifndef _TYPES_UINTX_T_
#define _TYPES_UINTX_T_
typedef unsigned char uint8_t;  /* 8 bits non signé */
typedef unsigned short int uint16_t;  /* 16 bits non signé */
typedef unsigned long int uint32_t;  /* 32 bits non signé */
typedef unsigned long long int uint64_t;  /* 64 bits non signé */
typedef uint32_t vaddr_t;
#endif

#ifndef _TYPES_BOOL_
#define _TYPES_BOOL_
typedef enum {FALSE=0, TRUE} bool;
#endif

#ifndef _TYPES_SIZE_T_
#define _TYPES_SIZE_T_
typedef uint32_t size_t;
typedef int ssize_t;
#endif


#define NULL ((void*)0)

#endif
