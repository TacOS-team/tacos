/**
 * @file cpu.h
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
 * @brief Définitions liées au fonctionnement interne du processeur
 */
#ifndef _CPU_H
#define _CPU_H

/* Définitions des masques pour les flags */
#define FLAGS_CF	0x00000001 /**< Carry Flag */
#define FLAGS_PF	0x00000004 /**< Parity Flag */
#define FLAGS_AF	0x00000010 /**< Adjust Flag */
#define FLAGS_ZF	0x00000040 /**< Zero Flag */
#define FLAGS_SF	0x00000080 /**< Sign Flag */
#define FLAGS_TF	0x00000100 /**< Trap Flag */
#define FLAGS_IF	0x00000200 /**< Interrupt enable Flag */
#define FLAGS_DF	0x00000400 /**< Direction Flag */
#define FLAGS_OF	0x00000800 /**< Overflow Flag */
#define FLAGS_IOPL1	0x00001000 /**< IO Privilege Level 1 */
#define FLAGS_IOPL2	0x00002000 /**< IO Privilege Level 1 */
#define FLAGS_NT	0x00004000 /**< Nested task Flag */
#define FLAGS_RF	0x00010000 /**< Resume Flag */
#define FLAGS_VM	0x00020000 /**< Virtual 8086 Mode Flag */
#define FLAGS_AC	0x00040000 /**< Alignment Check */
#define FLAGS_VIF	0x00080000 /**< Virtual interrupt Flag */
#define FLAGS_VIP	0x00100000 /**< Virtual interrupt Pending */
#define FLAGS_ID	0x00200000 /**< CPUID instruction */

#endif /* _CPU_H */
