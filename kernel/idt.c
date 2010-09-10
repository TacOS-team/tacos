/**
 * @file idt.c
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

/**
 * @file idt.c
 */

#include <types.h>
#include <idt.h>

/* Code inspiré de SOS. 
 * Permet d'initialiser la table des descripteurs d'interruption (et son registre associé)
 * Permet aussi d'associer un handler pour une interruption donnée. */

/* Il y a 256 interruptions possibles sur archi x86 (cf doc d'intel) */
#define IDT_ENTRIES_NUM 256

/**
 * @brief Entrée de l'IDT.
 * @struct x86_idt_entry
 */
struct x86_idt_entry
{
	/* Low dword */
	uint16_t offset_low;  /**< 15..0, offset of the routine in the segment */
	uint16_t seg_sel;     /**< 31..16, the ID of the segment */

	/* High dword */
	uint8_t reserved:5;   /**< 4..0 (unused) */
	uint8_t flags:3;      /**< 7..5 (unused) */
	uint8_t type:3;       /**< 10..8 (task gate 101, interrupt gate 110, trap gate 111...) */
	uint8_t op_size:1;    /**< 11 (0=16bits instructions, 1=32bits instr.) */
	uint8_t zero:1;       /**< 12 : Segment système */
	uint8_t dpl:2;        /**< 14..13 (niveau de privilège, 0 = superviseur, 3 = applicatif */
	uint8_t present:1;    /**< 15 (0 => Pas configuré) */
	uint16_t offset_high; /**< 31..16 */
} __attribute__((packed));

struct x86_idt_register
{
	uint16_t  limit; /* Taille de l'idt */
	uint32_t base_addr; /* Adresse de l'idt */
} __attribute__((packed, aligned (8)));

/* IDT */
static struct x86_idt_entry idt[IDT_ENTRIES_NUM]; /* On devrait peut être aligner à 8 octets ? */

void idt_setup()
{
	int i;
	struct x86_idt_register idtr; /* idt register */

	for (i = 0; i < IDT_ENTRIES_NUM; i++) {
		struct x86_idt_entry *idte = idt + i;

		/* Setup an empty IDTE interrupt gate, see figure 5-2 in Intel x86 doc, vol 3 */
		idte->seg_sel   = 1 << 3; // TODO !!!! Segment du kernel... à voir en détail...
		idte->reserved  = 0;
		idte->flags     = 0;
		idte->type      = 0x6; /* Interrupt gate (110b) */
		idte->op_size   = 1;   /* 32bits instructions */
		idte->zero      = 0;

		/* Aucun handler : */
		idt_disable_handler(i);
	}

	/* On configure le registre idt en lui donnant l'adresse de la table : */
	idtr.base_addr  = (uint32_t) idt;

	/* Et sa taille */
	idtr.limit = sizeof(idt) - 1;

	/* Commit the IDT into the CPU */
	asm volatile ("lidt %0\n"::"m"(idtr):"memory");
}

void idt_disable_handler(uint8_t index)
{
	idt_set_handler(index, (paddr_t)NULL, 0);
}

int idt_set_handler(uint8_t index, paddr_t handler_address, uint8_t priority)
{
	struct x86_idt_entry *idt_entry;

	if (priority > 3) {
		return -2;
	}


	idt_entry = &idt[index];

	if (handler_address == (paddr_t)NULL) {
		idt_entry->offset_low = 0;
		idt_entry->offset_high = 0;
		idt_entry->dpl = 0;
		idt_entry->present = 0;
	} else {
		/* Offset correspond à l'adresse du handler. Sauf qu'on découpe la valeur en 2 champs */
		idt_entry->offset_low = handler_address & 0xffff; /* 2 octets de poids faible */
		idt_entry->offset_high = (handler_address >> 16) ; /* 2 octets de poids fort */
		idt_entry->dpl = priority;
		idt_entry->present = 1;
	}

	return 0;
}

void idt_set_handler_type(uint8_t index, uint8_t trap_type)
{
	struct x86_idt_entry *idt_entry;

	idt_entry = &idt[index];	
	
	idt_entry->type = trap_type;
}

