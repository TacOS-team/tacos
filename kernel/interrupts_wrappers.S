/* Code repris de SOS...
	Copyright (C) 2004  The KOS Team
   Copyright (C) 1999  Free Software Foundation, Inc.
*/
         
.file "interrupts_wrappers.S"

.text

/* La table des handlers est definie dans interrupts.c. */
.extern interrupts_handler_array

/* C'est ici qu'on va declarer la table des wrappers */
.globl interrupts_wrapper_array

	
/* On a 2 cases a gerer : lorsque l'irq est sur le master ou sur le slave. */
/* irp permet d'iterer sur la liste qui suit */
.irp id, 0,1,2,3,4,5,6,7 /* Des ID ca serait cool. */
 
        .p2align 2, 0x90
        interrupt_wrapper_\id:
        .type interrupt_wrapper_\id,@function
 
                /* Fake error code */
                pushl $0
                /* Backup the context */
                pushl %ebp
                movl %esp, %ebp
 
                pushl %edi
                pushl %esi
                pushl %edx
                pushl %ecx
                pushl %ebx
                pushl %eax
                subl  $2,%esp
                pushw %ss
                pushw %ds
                pushw %es
                pushw %fs
                pushw %gs
 
               /* Call the handler with irq number as
                 * argument */
                pushl $\id
                leal  interrupts_handler_array,%edi
                call  *\id*4(%edi)
                addl  $4, %esp

                /* Restore the context */
                popw  %gs
                popw  %fs
                popw  %es
                popw  %ds
                popw  %ss
                addl  $2,%esp
                popl  %eax
                popl  %ebx
                popl  %ecx
                popl  %edx
                popl  %esi
                popl  %edi
                popl  %ebp

                /* Remove fake error code */
                addl $4, %esp

                /* La seule difference avec les exceptions : */
                /* Send EOI to PIC. See i8259 datasheet */
                movb $0x20, %al
                outb %al, $0x20

                iret
.endr 
 
/* sur le slave : */
.irp id, 8,9,10,11,12,13,14,15
 
        .p2align 2, 0x90
        interrupt_wrapper_\id:
        .type interrupt_wrapper_\id,@function
 
                /* ret eflags */
                /* ret cs */
                /* ret eip */
                /* Error code */

					 /* Fake error code */
					 pushl $0

                /* Backup the context */
                pushl %ebp
                movl %esp, %ebp
 
                pushl %edi
                pushl %esi
                pushl %edx
                pushl %ecx
                pushl %ebx
                pushl %eax
                subl  $2,%esp
                pushw %ss
                pushw %ds
                pushw %es
                pushw %fs
                pushw %gs

                /* Call the handler with interrupt number as
                 * argument */
                pushl $\id
                leal  interrupts_handler_array,%edi
                call  *\id*4(%edi)
                addl  $4, %esp

                /* Restore the context */
                popw  %gs
                popw  %fs
                popw  %es
                popw  %ds
                popw  %ss
                addl  $2,%esp
                popl  %eax
                popl  %ebx
                popl  %ecx
                popl  %edx
                popl  %esi
                popl  %edi
                popl  %ebp
 
                /* Error code isn't compatible with iretd */
                addl $4, %esp
 
                /* EOI */
                movb $0x20, %al
                outb %al, $0xa0 /* PIC2 */
                outb %al, $0x20 /* PIC1 */

                iret
.endr


/* On construit la table des wrappers en y inserant 
tous les wrappers qu'on vient de construire. */
.section ".rodata"
.p2align 5, 0x0
interrupts_wrapper_array:
	.irp id, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	  .long (interrupt_wrapper_\id)
	.endr
