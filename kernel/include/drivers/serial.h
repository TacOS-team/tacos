/**
 * @file serial.h
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
 * Serial communication driver
 */

#ifndef _SERIAL_H
#define _SERIAL_H

/**
 * @enum serial_port
 * @brief Ports de communication
 */
typedef enum {
	COM1, COM2, COM3, COM4
}serial_port;

/**
 * @brief Handler d'interruption du driver série
 */
void serial_isr(int id);

/**
 * @brief Fonction d'initialisation du driver série
 */
int serial_init();

#endif /* _SERIAL_H_ */
