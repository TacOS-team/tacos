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
