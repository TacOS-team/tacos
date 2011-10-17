#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <process.h>

static inline void RESTORE_CONTEXT() __attribute__((always_inline));
static inline void PUSH_CONTEXT() __attribute__((always_inline));

static inline void RESTORE_CONTEXT(){
		asm(
		/*On dépile les registres pour les mettre en place */
		 "pop %gs\n\t"
		 "pop %fs\n\t"
		 "pop %es\n\t"
		 "pop %ds\n\t"
		 "pop %edi\n\t"
		 "pop %esi\n\t"
		 "pop %ebp\n\t"
		 "pop %ebx\n\t"
		 "pop %edx\n\t"
		 "pop %ecx\n\t"
		 "pop %eax\n\t"
		 
		 /* Et on switch! (enfin! >_<) */
		 "iret\n\t"
	 );
} 

static inline void PUSH_CONTEXT(process_t* current) {
	asm(
			"push %0\n\t"
			"push %1\n\t"
			"push %2\n\t"
			"push %3\n\t"
			"push %4\n\t"
			"push %5\n\t"
			::"m" (current->regs.eax),
			"m" (current->regs.ecx),
			"m" (current->regs.edx),
			"m" (current->regs.ebx),
			"m" (current->regs.ebp),
			"m" (current->regs.esi)
	);
	asm(
			"push %0\n\t"
			"push %1\n\t"
			"push %2\n\t"
			"push %3\n\t"
			"push %4\n\t"
			::
			"m" (current->regs.edi),	
			"m" (current->regs.ds),		
			"m" (current->regs.es),		
			"m" (current->regs.fs),		
			"m" (current->regs.gs)		
	);
}


#endif /* _CONTEXT_H */
