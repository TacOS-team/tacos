/**
 * @file beeper.c
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

#include <drivers/beeper.h>
#include <types.h>
#include <ioports.h>
#include <fs/devfs.h>

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
		outb(tmp | 3, 0x61);
	}
}
 
//make it shutup
static void nosound()
{
	uint8_t tmp = (inb(0x61) & 0xFC);

	outb(tmp, 0x61);
}

static int frequences[2][7] = {{131, 147, 165, 175, 196, 220, 247},
														 {262, 294, 330, 349, 392, 440, 446}};

static size_t beeper_write(open_file_descriptor* ofd __attribute__((unused)), const void* b, size_t count) {
	unsigned char* buf = (unsigned char*)b;
	// NOTE(0-3) OCTAVE(4-6) 0(7)
	// DUREE(0-6)						 1(7)
	#define IS_NOTE(x) ((x & 128) == 0)
	int note;
	int octave;
	int duree;
	size_t i;
	for (i = IS_NOTE(buf[0]) ? 0 : 1; i < count; i++) {
		if (IS_NOTE(buf[i])) {
			note = buf[i] & 0x0F;
			octave = (buf[i] & 0xF0) >> 4;
		} else {
			duree = buf[i] & 0x7F;
			play_sound(frequences[octave][note]);
			int j;
			for (j=1; j < 20000000 * duree; j++);
			nosound();
		}
	}
	return i;
}

static chardev_interfaces di = {
	.read = NULL,
	.write = beeper_write,
	.open = NULL,
	.close = NULL,
	.ioctl = NULL
};

void beeper_init() {
	register_chardev("beeper", &di);
}
