/**
 * @file idt.h
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

#ifndef _IDT_H_
#define _IDT_H_

/**
 * @file idt.h
 * @brief Initialisation de la table des descripteurs d'interruption 
 * (et exceptions) et association aux handlers.
 *
 */

/**
 *  Nombre d'exceptions : 
 */
#define EXCEPTION_NUM 32
/**
 *  On va définir une constante pour dire où on va mapper (pour l'idt) 
 */
#define EXCEPTION_BASE 0
/**
 *  Nombre d'interruptions : 
 */
#define INTERRUPT_NUM 32
/**
 *  Définition de là où on mappe les interruptions. 
 */
#define INTERRUPT_BASE 32 /* Juste après les exceptions */


#define IDT_TRAPGATE		0x7
#define IDT_INTGATE		0x6
#define IDT_TASKGATE 	0x5

#include <types.h>

/** 
 * @brief Initialisation de l'IDT.
 *
 * Initialisation de l'IDT avec des champs par défaut mais aucun handler.
 * L'IDT est envoyée au CPU.
 */
void idt_setup();

/** 
 * @brief Associe un handler à une interruption/exception.
 * 
 * @param index L'index dans l'IDT.
 * @param handler_address L'adresse du handler.
 * @param priority Le niveau de privilège dans lequel va s'exécuter l'interruption.
 * 
 * @return 0 en cas de succès.
 */
int idt_set_handler(uint8_t index, paddr_t handler_address, uint8_t priority);

/** 
 * @brief Change le type d'une interruption (interrupt gate, trap gate, task 
 * gate).
 * 
 * @param index L'index dans l'IDT.
 * @param trap_type Le type de gate.
 */
void idt_set_handler_type(uint8_t index, uint8_t trap_type);

/** 
 * @brief Désactive une interruption.
 * 
 * @param index L'index dans l'IDT.
 */
void idt_disable_handler(uint8_t index);

#endif
