/**
 * @file vm86.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

#include <debug.h>
#include <gdt.h>
#include <types.h>

#define	CLI	0xFA
#define	STI	0xFB
#define	INTxx	0xCD
#define	INTO	0xCE
#define	IRET	0xCF
#define	OPSIZ	0x66
#define	INT3	0xCC
#define	LOCK	0xF0
#define	PUSHF	0x9C
#define	POPF	0x9D

int check_vm86()
{
	int ret = 0;
	asm volatile(
		"smsw %%ax\n\t"
		"and $1, %%eax\n\t"
		"mov %%eax, %0"
		:"=m"(ret));
	return ret;
}
	
uint16_t ustack[64];
uint16_t sstack[64];
paddr_t ss_addr = (paddr_t)(sstack + 63);
paddr_t us_addr = (paddr_t)(ustack + 63);
					
void exec_vm86_task(paddr_t task)
{
	BOCHS_BREAKPOINT;
	task = 0x500;
	asm(
		"cli\n\t"
		"push $0x23\n\t"                //SS
		"push %3\n\t"           //ESP
		"pushfl\n\t"
		"popl %%eax\n\t"
		"orl $0x00024200, %%eax\n\t"
		"push %%eax\n\t"        // Flags
		"push $0x1B\n\t"        //CS
		"push %0\n\t"           //EIP
		"movl %2, %1\n\t"       // Sauvegarde de la pile kernel dans la TSS
		"movw $0x20, %%ax\n\t" 
		"movw %%ax, %%ds\n\t"
		"iret"
		:"=m"(task), "=m"(get_default_tss()->esp):"a"(ss_addr), "b"(us_addr));
}

void quit_vm86()
{
	
	
	
}
