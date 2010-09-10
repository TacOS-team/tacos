/**
 * @file beeper.c
 *
 * @author TacOS developers 
 *
 * Maxime Cheramy <maxime81@gmail.com>
 * Nicolas Floquet <nicolasfloquet@gmail.com>
 * Benjamin Hautbois <bhautboi@gmail.com>
 * Ludovic Rigal <ludovic.rigal@gmail.com>
 * Simon Vernhes <simon@vernhes.eu>
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

#include <beeper.h>
#include <types.h>
#include <ioports.h>

static void set_PIT_2(uint32_t frequency)
{
 	uint32_t div;
    //Set the PIT to the desired frequency
 	div = 1193180 / frequency;
 	outb(0xb6, 0x43);
 	outb((uint8_t) (div), 0x42);
 	outb((uint8_t) (div >> 8), 0x42);
}

//Play sound using built in speaker
static void play_sound(uint32_t nFrequence)
{
 	uint8_t tmp;
 
    set_PIT_2(nFrequence);
 
    //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
 }
 
 //make it shutup
 static void nosound()
 {
 	uint8_t tmp = (inb(0x61) & 0xFC);
 
 	outb(0x61, tmp);
 }
 
 //Make a beep
 void beep()
 {
	 int i;
 	 play_sound(1000);
 	 for(i=0 ; i<10000000 ; i++);
 	 nosound();
     //set_PIT_2(old_frequency);
 }
