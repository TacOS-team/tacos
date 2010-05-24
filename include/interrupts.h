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
