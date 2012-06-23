/**
 * @file exception.h
 *
 * @author TacOS developers 
 *
 * @section LICENSE
 *
 * Copyright (C) 2010, 2011, 2012 - TacOS developers.
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

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

/* Define des différentes exceptions possibles : */

#define EXCEPTION_DIVIDE_ERROR                  0 /**< Division par zero. */
#define EXCEPTION_DEBUG                         1 /**< Debug exception. */
#define EXCEPTION_NMI_INTERRUPT                 2 /**< Interruption non masquable. */
#define EXCEPTION_BREAKPOINT                    3 /**< Point d'arrêt. */
#define EXCEPTION_OVERFLOW                      4 /**< Overflow. */
#define EXCEPTION_BOUND_RANGE_EXCEDEED          5 /**< Levé par l'instruction bound. */
#define EXCEPTION_INVALID_OPCODE                6 /**< Opcode invalide. */
#define EXCEPTION_DEVICE_NOT_AVAILABLE          7 /**< FPU instruction sans FPU. */
#define EXCEPTION_DOUBLE_FAULT                  8 /**< Double faute. */
#define EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN   9 /**< Coprocessor Segment Overrun. */
#define EXCEPTION_INVALID_TSS                  10 /**< Invalid segment selector. */
#define EXCEPTION_SEGMENT_NOT_PRESENT          11 /**< Segment non présent (bit present à 0). */
#define EXCEPTION_STACK_SEGMENT_FAULT          12 /**< Stack segment fault. */
#define EXCEPTION_GENERAL_PROTECTION           13 /**< General Protection Fault. (segment, droits, etc) */
#define EXCEPTION_PAGE_FAULT                   14 /**< Page fault. */
#define EXCEPTION_INTEL_RESERVED_1             15 /**< Réservé. */
#define EXCEPTION_FLOATING_POINT_ERROR         16 /**< Floating point error. */
#define EXCEPTION_ALIGNEMENT_CHECK             17 /**< Défaut d'alignement. */
#define EXCEPTION_MACHINE_CHECK                18 /**< Détection d'erreur interne. */
#define EXCEPTION_INTEL_RESERVED_2             19 /**< Réservé. */ 
#define EXCEPTION_INTEL_RESERVED_3             20 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_4             21 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_5             22 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_6             23 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_7             24 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_8             25 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_9             26 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_10            27 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_11            28 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_12            29 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_13            30 /**< Réservé. */
#define EXCEPTION_INTEL_RESERVED_14            31 /**< Réservé. */

#ifndef ASM_SOURCE

#include <types.h>

/**
 * Définition du type pour un handler d'exception.
 */
typedef void (*exception_handler_t)(uint32_t error_id, uint32_t error_code);

/**
 * Définit le handler associé à une exception.
 */
int exception_set_routine(uint8_t exception_id, exception_handler_t routine);
int exception_disable(uint8_t exception_id);

#endif

#endif
