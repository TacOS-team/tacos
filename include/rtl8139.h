/**
 * @file rtl8139.h
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
 * @brief Ancien driver réseau. Inutilisé actuellement.
 */

#ifndef _RTL8139_H
#define _RTL8139_H

/**
 * @brief Initialise le driver
 * Son rôle principal est de chercher le périphérique sur le bus PCI et de sauvegardé son adresse
 * @return 0 pour un succès, -1 sinon
 */
int rtl8139_driver_init();

int send_packet(char* packet, uint32_t length);

void rtl8139_isr(int id);

void rtl8139_debug_info();

#endif /*_RTL8139_H */
