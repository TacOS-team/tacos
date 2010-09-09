/*      rtl8139.h
      
      Copyright 2010 Nicolas Floquet <nicolasfloquet@gmail.com>
      
      This program is free software; you can redistribute it and/or modify
      it under the terms of the GNU General Public License as published by
      the Free Software Foundation; either version 2 of the License, or
      (at your option) any later version.
      
      This program is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
      GNU General Public License for more details.
      
      You should have received a copy of the GNU General Public License
      along with this program; if not, write to the Free Software
      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
      MA 02110-1301, USA.
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
