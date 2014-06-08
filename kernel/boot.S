#define ASM_FILE        1
#include <multiboot.h>

/* C symbol format. HAVE_ASM_USCORE is defined by configure. */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)                     _ ## sym
#else
# define EXT_C(sym)                     sym
#endif

/* The size of our stack (16KB). */
#define STACK_SIZE                      0x4000

/* The flags for the Multiboot header. */
#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS         0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS         0x00010003
#endif

.globl  start, _start
        start:
        _start:
jmp     multiboot_entry

.section .__mbHeader
        /* Align 32 bits boundary. */
        .align  4
        /* Multiboot header. */
multiboot_header:
        /* magic */
        .long   MULTIBOOT_HEADER_MAGIC
        /* flags */
        .long   MULTIBOOT_HEADER_FLAGS
        /* checksum */
        .long   -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
#ifndef __ELF__
        /* header_addr */
        .long   multiboot_header
        /* load_addr */
        .long   __b_load
        /* load_end_addr */
        .long   __e_load
        /* bss_end_addr */
        .long   __e_kernel
        /* entry_addr */
        .long   multiboot_entry
#endif /* ! __ELF__ */

.text

multiboot_entry:
        /* Initialize the stack pointer. */
        movl    $(stack + STACK_SIZE), %esp

        /* Reset EFLAGS. */
        pushl   $0
        popf

        /* Push the pointer to the Multiboot information structure. */
        pushl   %ebx
        /* Push the magic value. */
        pushl   %eax

        /* Now enter the C main function... */
        call    EXT_C(cmain)

        /* Halt. */
        pushl   $halt_message
        call    EXT_C(kprintf)

loop:   hlt
        jmp     loop

halt_message:
        .asciz  "Halted."

        /* Our stack area. */
        .comm   stack, STACK_SIZE
