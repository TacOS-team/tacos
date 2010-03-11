#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

/* Define des différentes exceptions possibles : */

#define EXCEPTION_DIVIDE_ERROR                  0
#define EXCEPTION_DEBUG                         1
#define EXCEPTION_NMI_INTERRUPT                 2
#define EXCEPTION_BREAKPOINT                    3
#define EXCEPTION_OVERFLOW                      4
#define EXCEPTION_BOUND_RANGE_EXCEDEED          5
#define EXCEPTION_INVALID_OPCODE                6
#define EXCEPTION_DEVICE_NOT_AVAILABLE          7
#define EXCEPTION_DOUBLE_FAULT                  8
#define EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN   9
#define EXCEPTION_INVALID_TSS                  10
#define EXCEPTION_SEGMENT_NOT_PRESENT          11
#define EXCEPTION_STACK_SEGMENT_FAULT          12
#define EXCEPTION_GENERAL_PROTECTION           13
#define EXCEPTION_PAGE_FAULT                   14
#define EXCEPTION_INTEL_RESERVED_1             15
#define EXCEPTION_FLOATING_POINT_ERROR         16
#define EXCEPTION_ALIGNEMENT_CHECK             17
#define EXCEPTION_MACHINE_CHECK                18
#define EXCEPTION_INTEL_RESERVED_2             19
#define EXCEPTION_INTEL_RESERVED_3             20
#define EXCEPTION_INTEL_RESERVED_4             21
#define EXCEPTION_INTEL_RESERVED_5             22
#define EXCEPTION_INTEL_RESERVED_6             23
#define EXCEPTION_INTEL_RESERVED_7             24
#define EXCEPTION_INTEL_RESERVED_8             25
#define EXCEPTION_INTEL_RESERVED_9             26
#define EXCEPTION_INTEL_RESERVED_10            27
#define EXCEPTION_INTEL_RESERVED_11            28
#define EXCEPTION_INTEL_RESERVED_12            29
#define EXCEPTION_INTEL_RESERVED_13            30
#define EXCEPTION_INTEL_RESERVED_14            31

#ifndef ASM_SOURCE

#include <types.h>

typedef void (*exception_handler_t)(int error_id);

int exception_set_routine(uint8_t exception_id, exception_handler_t routine);
int exception_disable(uint8_t exception_id);

#endif

#endif
