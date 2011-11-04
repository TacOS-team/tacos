/**
 * @file context.h
 *
 * @author TacOS developers 
 *
 *
 * @section LICENSE
 *
 * Copyright (C) 2010 - TacOS developers.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 * @section DESCRIPTION
 *
 * Description de ce que fait le fichier
 */

#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <kprocess.h>

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
