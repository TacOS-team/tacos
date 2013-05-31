/**
 * @file floppy_motor.h
 *
 * @author TacOS developers 
 *
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

#ifndef _FLOPPY_MOTOR_H_
#define _FLOPPY_MOTOR_H_

/**
 * @enum floppy_motor_state
 * @brief Décrit l'état du moteur (on/off)
 */
typedef enum { OFF = 0, ON = 1 } floppy_motor_state ;

/**
 * @brief Change l'état du moteur
 * @param drive Numero de drive
 * @param new_state état du moteur désiré
 */
void floppy_motor(int drive, floppy_motor_state new_state);

#endif
