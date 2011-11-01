/**
 * @file exception.h
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

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

/* Define des différentes exceptions possibles : */

#define EXCEPTION_DIVIDE_ERROR                  0
#define EXCEPTION_DEBUG                         1
#define EXCEPTION_NMI_INTERRUPT                 2
#define EXCEPTION_BREAKPOINT                    3
#define EXCEPTION_OVERFLOW                      4
#define EXCEPTION_BOUND_RANGE_EXCEDEED          5
#define EXCEPTION_INVALID_OPCODE                6
#define EXCEPTION_DEVICE_NOT_AVAILABLE          7
#define EXCEPTION_DOUBLE_FAULT                  8
#define EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN   9
#define EXCEPTION_INVALID_TSS                  10
#define EXCEPTION_SEGMENT_NOT_PRESENT          11
#define EXCEPTION_STACK_SEGMENT_FAULT          12
#define EXCEPTION_GENERAL_PROTECTION           13
#define EXCEPTION_PAGE_FAULT                   14
#define EXCEPTION_INTEL_RESERVED_1             15
#define EXCEPTION_FLOATING_POINT_ERROR         16
#define EXCEPTION_ALIGNEMENT_CHECK             17
#define EXCEPTION_MACHINE_CHECK                18
#define EXCEPTION_INTEL_RESERVED_2             19
#define EXCEPTION_INTEL_RESERVED_3             20
#define EXCEPTION_INTEL_RESERVED_4             21
#define EXCEPTION_INTEL_RESERVED_5             22
#define EXCEPTION_INTEL_RESERVED_6             23
#define EXCEPTION_INTEL_RESERVED_7             24
#define EXCEPTION_INTEL_RESERVED_8             25
#define EXCEPTION_INTEL_RESERVED_9             26
#define EXCEPTION_INTEL_RESERVED_10            27
#define EXCEPTION_INTEL_RESERVED_11            28
#define EXCEPTION_INTEL_RESERVED_12            29
#define EXCEPTION_INTEL_RESERVED_13            30
#define EXCEPTION_INTEL_RESERVED_14            31

#ifndef ASM_SOURCE

#include <types.h>

typedef void (*exception_handler_t)(uint32_t error_id, uint32_t error_code);

int exception_set_routine(uint8_t exception_id, exception_handler_t routine);
int exception_disable(uint8_t exception_id);

#endif

#endif
