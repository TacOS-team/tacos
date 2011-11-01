/**
 * @file fpu.c
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

#include <fpu.h>
#include <interrupts.h>
#include <klog.h>

static void FPU_handler(int id __attribute__ ((unused)))
{
  kdebug("FPU Exception");
}

void init_fpu()
{
	interrupt_set_routine(IRQ_COPROCESSOR, FPU_handler, 0);
  // FINIT : Initialize FPU after checking for pending unmasked floating-point exceptions.
  asm("finit");
}
