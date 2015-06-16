/**
 * @file interrupts_wrappers.S
 * @brief Wrapper pour l'execution des interruptions.
 *
 * Code assembleur qui encapsule le lancement de la routine associée. 
 * Structure inspirée de SOS (Copyright (C) 2004  The KOS Team, 
 * Copyright (C) 1999  Free Software Foundation, Inc.)
*/
         
.file "interrupts_wrappers.S"

.text

/* La table des handlers est definie dans interrupts.c. */
.extern interrupts_handler_array

.extern do_schedule

/* C'est ici qu'on va declarer la table des wrappers */
.globl interrupts_wrapper_array

	
/* On a 2 cas à gerer : lorsque l'irq est sur le master ou sur le slave. */
/* irp permet d'iterer sur la liste qui suit */
.irp id, 0,1,2,3,4,5,6,7 /* Des ID ca serait cool. */
 
        .p2align 2, 0x90
        interrupt_wrapper_\id:
        .type interrupt_wrapper_\id,@function
 
					pusha
					push %gs
					push %fs
					push %ds
					push %es
					
					/* On repasse le bon segment de data au kernel */
					push %ebx
					mov $0x10, %bx
					mov %bx, %ds
					pop %ebx


					/**
					 * Appelle le handler qui a été enregistré dans le tableau interrupts_handler_array.
					 */
					pushl $\id
					leal  interrupts_handler_array,%edi
					call  *\id*4(%edi)
					addl  $4, %esp

					.ifeq \id
					push %ebp
					mov %esp,%ebp
					call do_schedule
					pop %ebp
					.endif

               /* La seule difference avec les exceptions : */
               /* Send EOI to PIC. See i8259 datasheet */
               movb $0x20, %al
               outb %al, $0x20


					pop %es
					pop %ds
					pop %fs
					pop %gs
					popa

               iret
.endr 
 
/* sur le slave : */
.irp id, 8,9,10,11,12,13,14,15
 
        .p2align 2, 0x90
        interrupt_wrapper_\id:
        .type interrupt_wrapper_\id,@function
 
					pusha
					push %gs
					push %fs
					push %ds
					push %es
					
					/* On repasse le bon segment de data au kernel */
					push %ebx
					mov $0x10,%bx
					mov %bx,%ds
					pop %ebx

					/**
					 * Appelle le handler qui a été enregistré dans le tableau interrupts_handler_array.
					 */
					pushl $\id
					leal  interrupts_handler_array,%edi
					call  *\id*4(%edi)
               /* La seule difference avec les exceptions : */
               /* Send EOI to PIC. See i8259 datasheet */
               movb $0x20, %al
               outb %al, $0xa0 /* PIC2 */
               outb %al, $0x20 /* PIC1 */

					addl  $4, %esp

					pop %es
					pop %ds
					pop %fs
					pop %gs
					popa

               iret
.endr

/* Interruptions logicielles : */
.irp id, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
 
        .p2align 2, 0x90
        interrupt_wrapper_\id:
        .type interrupt_wrapper_\id,@function
 
					pusha
					push %gs
					push %fs
					push %ds
					push %es
					
					/* On repasse le bon segment de data au kernel */
					push %ebx
					mov $0x10,%bx
					mov %bx,%ds
					pop %ebx

					/* Appelle le handler */
					pushl $\id
					leal  interrupts_handler_array,%edi
					call  *\id*4(%edi)
					addl  $4, %esp

					.ifeq \id-16
					push %ebp
					mov %esp,%ebp
					call do_schedule
					pop %ebp
					.endif

					pop %es
					pop %ds
					pop %fs
					pop %gs
					popa

               iret
.endr



/* On construit la table des wrappers en y inserant 
tous les wrappers qu'on vient de construire. */
.section ".rodata"
.p2align 5, 0x0
interrupts_wrapper_array:
	.irp id, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	  .long (interrupt_wrapper_\id)
	.endr
