/* 
	Copyright (C) 2004  The KOS Team
   Copyright (C) 1999  Free Software Foundation, Inc.
*/
#include "exception.h"
         
.file "exception_wrappers.S"

.text

/* La table des handlers est definie dans exception.c. */
.extern exception_handler_array

/* C'est ici qu'on va declarer la table des wrappers */
.globl exception_wrapper_array

	
/**
 * For exceptions with/without error code, refer to Intel x86 doc vol 3,
 * section 5.12
 */

/* On va definir les wrappers des exceptions sans code retour. */
/* irp permet d'iterer sur la liste qui suit */
.irp id,                              \
         EXCEPTION_DIVIDE_ERROR,                \
         EXCEPTION_DEBUG,                       \
         EXCEPTION_NMI_INTERRUPT,               \
         EXCEPTION_BREAKPOINT,                  \
         EXCEPTION_OVERFLOW,                    \
         EXCEPTION_BOUND_RANGE_EXCEDEED,        \
         EXCEPTION_INVALID_OPCODE,              \
         EXCEPTION_DEVICE_NOT_AVAILABLE,        \
         EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN, \
         EXCEPTION_INTEL_RESERVED_1,            \
         EXCEPTION_FLOATING_POINT_ERROR,        \
         EXCEPTION_MACHINE_CHECK,               \
         EXCEPTION_INTEL_RESERVED_2,            \
         EXCEPTION_INTEL_RESERVED_3,            \
         EXCEPTION_INTEL_RESERVED_4,            \
         EXCEPTION_INTEL_RESERVED_5,            \
         EXCEPTION_INTEL_RESERVED_6,            \
         EXCEPTION_INTEL_RESERVED_7,            \
         EXCEPTION_INTEL_RESERVED_8,            \
         EXCEPTION_INTEL_RESERVED_9,            \
         EXCEPTION_INTEL_RESERVED_10,           \
         EXCEPTION_INTEL_RESERVED_11,           \
         EXCEPTION_INTEL_RESERVED_12,           \
         EXCEPTION_INTEL_RESERVED_13,           \
         EXCEPTION_INTEL_RESERVED_14
 
        .p2align 2, 0x90
        exception_wrapper_\id:
        .type exception_wrapper_\id,@function
					pusha
					push %gs
					push %fs
					push %ds
					push %es

					/* Call the handler with exception number as
					 * argument */
					pushl $0
					pushl $\id
					leal  exception_handler_array,%edi
					call  *\id*4(%edi)
					addl  $8, %esp
					
					pop %es
					pop %ds
					pop %fs
					pop %gs
					popa

					sti
					iret
.endr 
 
        /* On fait la meme chose avec les exceptions avec code retour */
.irp id,                              \
	EXCEPTION_INVALID_TSS,                  \
	EXCEPTION_SEGMENT_NOT_PRESENT,          \
	EXCEPTION_STACK_SEGMENT_FAULT,          \
	EXCEPTION_GENERAL_PROTECTION,           \
	EXCEPTION_PAGE_FAULT,                   \
	EXCEPTION_DOUBLE_FAULT,                 \
	EXCEPTION_ALIGNEMENT_CHECK
 
        .p2align 2, 0x90
        exception_wrapper_\id:
        .type exception_wrapper_\id,@function
 					pusha
					push %gs
					push %fs
					push %ds
					push %es

					/* Call the handler with exception number as
					 * argument */
					addl $48, %esp /* J'en suis pas fier. C'est pour recuperer le code d'erreur */
					popl %edi
					subl $44, %esp
					pushl %edi
					pushl $\id
					leal  exception_handler_array,%edi
					call  *\id*4(%edi)
					addl  $8, %esp
					
					pop %es
					pop %ds
					pop %fs
					pop %gs
					popa

					/* Error code isn't compatible with iretd */
					addl $4, %esp

					sti
					iret
.endr


/* On construit la table des wrappers en y inserant 
tous les wrappers qu'on vient de construire. */
.section ".rodata"
.p2align 5, 0x0
exception_wrapper_array:
	.irp id, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15, \
	         16,17,18,19,20,21,22,23,24,25,26,27,29,30,31
	  .long (exception_wrapper_\id)
	.endr
