/**
 * @file interrupts.h
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

#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

/**
 * @file interrupts.h
 * @brief Gestion des interruptions..
 */

/**
 * Définition des IRQ :
 */
#define IRQ_TIMER           0
#define IRQ_KEYBOARD        1
#define IRQ_SLAVE_PIC       2
#define IRQ_COM2            3
#define IRQ_COM1            4
#define IRQ_RESERVED_1      5 // sound card
#define IRQ_FLOPPY          6
#define IRQ_LPT1            7
#define IRQ_8_REAL_TIME_CLK 8
#define IRQ_REDIRECT_IRQ2   9
#define IRQ_RESERVED_2      10
#define IRQ_RESERVED_3      11
#define IRQ_PS2_MOUSE       12
#define IRQ_COPROCESSOR     13
#define IRQ_HARDDISK        14
#define IRQ_RESERVED_4      15
#define IRQ_SYSCALL			16

#include <types.h>


/* Structement représentant comment les registres sont empilés lors d'une interruption */
typedef struct {
	uint32_t interrupt_id;
	
	/* Registres */
	uint16_t es;
	uint16_t es2;
	uint16_t ds;
	uint16_t ds2;
	uint16_t gs;
	uint16_t gs2;
	uint16_t fs;
	uint16_t fs2;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t kesp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t esp;
	uint16_t ss;
	
}intframe;

typedef void (*interrupt_handler_t)(int interrupt_id);

/** 
 * @brief Ajoute une nouvelle interruption.
 *
 * Ajoute une nouvelle interruption en mappant le numéro de l'interruption à 
 * la routine d'exécution.
 * 
 * @param interrupt_id Le numéro d'interruption.
 * @param routine La routine qui sera exécutée lors de cette interruption.
 * @param privilege Le niveau de privilège/priorité (0 ou 3).
 * 
 * @return 0 en cas de succès. Autre chose sinon.
 */
int interrupt_set_routine(uint8_t interrupt_id, interrupt_handler_t routine, uint8_t privilege);

/** 
 * @brief Désactive une interruption.
 * 
 * @param interrupt_id Le numéro de l'interruption à désactiver.
 * 
 * @return 0 en cas de succès. Autre chose sinon.
 */
int interrupt_disable(uint8_t interrupt_id);

/** 
 * @brief Change le type d'une interruption en une trapgate.
 * 
 * @param interrupt_id Le numéro de l'interruption dont on veut changer le 
 * type pour en faire une trapgate.
 */
void make_trapgate_from_int(uint8_t interrupt_id);

#endif
