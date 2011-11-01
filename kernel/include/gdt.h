/**
 * @file gdt.h
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

#ifndef _GDT_H_
#define _GDT_H_

#include <types.h>

#define KERNEL_CODE_SEGMENT		0x08
#define KERNEL_DATA_SEGMENT		0x10
#define KERNEL_STACK_SEGMENT	(KERNEL_DATA_SEGMENT)


#define USER_CODE_SEGMENT		0x1B
#define USER_DATA_SEGMENT		0x23
#define USER_STACK_SEGMENT		(USER_DATA_SEGMENT)



//#define SIZE_OF_GDT_ENTRY 8

/*
 * Segment Descriptor (cf doc intel v3. 3.4.3)
 */
struct x86_segment_descriptor
{
	/* Double mot de poids faible */
	uint16_t segment_limit_15_0; /* Segment Limit 15:00. Spécifie la taille du segment. 
											  Le proc concatene les 2 champs segment limit pour 
											  former une valeur de 20 bits. */
	uint16_t base_address_15_0;  /* Défini la position de l'octet 0 du segment 
										 	  dans les 4Gio de l'espace adressable linéaire */

	/* Double mot de poids fort */
	uint8_t base_address_23_16; /* Suite de la base address. */
	uint8_t segment_type:4; /* Cf 3.4.3.1 de la doc intel ! */
	uint8_t descriptor_type:1; /* 0 = system segment, 1 = code ou data segment */
	uint8_t dpl:2; /* Descriptor Privilege Level. 0 à 3. 0 pour le kernel et 3 pour les applis */
	uint8_t present:1; /* Indique si le segment est présent en mémoire (1) ou non (0) */
	uint8_t segment_limit_19_16:4; /* Suite du segment limit. */
	uint8_t available:1; /* Un bit disponible pour nous... */
	uint8_t zero:1; /* Bit réservé qui doit prendre la valeur 0. */
	uint8_t operation_size:1; /* 0 = 16bits, 1 = 32bits */
	uint8_t granularity:1; /* Granularité : Détermine l'échele du segment limit. 0 : c'est des bytes, 1 : c'est par 4kio */
	uint8_t base_address_31_24; /* Fin de la base address. Soit 32 bits ce qui correspond aux 4Gio adressables. */

} __attribute__ ((packed, aligned (8)));



typedef struct tss {
    uint16_t    previous_task, __previous_task_unused;
    uint32_t    esp0;
    uint16_t    ss0, __ss0_unused;
    uint32_t    esp1;
    uint16_t    ss1, __ss1_unused;
    uint32_t    esp2;
    uint16_t    ss2, __ss2_unused;
    uint32_t    cr3;
    uint32_t    eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint16_t    es, __es_unused;
    uint16_t    cs, __cs_unused;
    uint16_t    ss, __ss_unused;
    uint16_t    ds, __ds_unused;
    uint16_t    fs, __fs_unused;
    uint16_t    gs, __gs_unused;
    uint16_t    ldt_selector, __ldt_sel_unused;
    uint16_t    debug_flag, io_map;
} __attribute__ ((packed)) tss_t;


//pas testée encore (faudrait pouvoir intialiser une TSS pour ca)
// TODO : Expliquer ce que du code C fiche dans ce header.
static inline void jmp_to_tss(uint16_t tss_sel)
{
	static struct {
	uint32_t eip ;
	uint16_t cs  ;
	} __attribute__ ((packed)) tss_link = {0, 0};

	tss_link.cs = tss_sel; // Selecteur de la TSS dans la GDT

	// Changmement de contexte Hardware avec l'instruction 'ljmp'
	asm volatile ("ljmp %0" : : "m" (tss_link));
}


void gdt_setup(size_t ram_size);
void init_tss(uint32_t stack_address);
tss_t* get_default_tss();

#endif
