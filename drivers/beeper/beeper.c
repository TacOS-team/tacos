/**
 * @file beeper.c
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

#include <beeper.h>
#include <types.h>
#include <ioports.h>
#include <scheduler.h>

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

static void note_DO(int octave, int duree) {
	int j;
	play_sound(frequences[octave][0]);
	for (j=1; j < 20000000 * duree; j++);
	nosound();
}

static void note_RE(int octave, int duree) {
	int j;
	play_sound(frequences[octave][1]);
	for (j=1; j < 20000000 * duree; j++);
	nosound();
}

static void note_MI(int octave, int duree) {
	int j;
	play_sound(frequences[octave][2]);
	for (j=1; j < 20000000 * duree; j++);
	nosound();
}

static void  __attribute__ ((unused))	note_FA(int octave, int duree) { 
	int j;
	play_sound(frequences[octave][3]);
	for (j=1; j < 20000000 * duree; j++);
	nosound();
}

static void note_SOL(int octave, int duree) {
	int j;
	play_sound(frequences[octave][4]);
	for (j=1; j < 20000000 * duree; j++);
	nosound();
}

static void note_LA(int octave, int duree) {
	int j;
	play_sound(frequences[octave][5]);
	for (j=1; j < 20000000 * duree; j++);
	nosound();
}

static void note_SI(int octave, int duree) {
	int j;
	play_sound(frequences[octave][6]);
	for (j=1; j < 20000000 * duree; j++);
	nosound();
}

//Make a beep
void beep()
{
	// Play Au clair de la lune.
	note_DO(1, 1);
	note_DO(1, 1);
	note_DO(1, 1);
	note_RE(1, 1);
	note_MI(1, 2);
	note_RE(1, 2);

	note_DO(1, 1);
	note_MI(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_DO(1, 4);

	note_DO(1, 1);
	note_DO(1, 1);
	note_DO(1, 1);
	note_RE(1, 1);
	note_MI(1, 2);
	note_RE(1, 2);

	note_DO(1, 1);
	note_MI(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_DO(1, 4);

	note_RE(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_LA(0, 2);
	note_LA(0, 2);

	note_RE(1, 1);
	note_DO(1, 1);
	note_SI(0, 1);
	note_LA(0, 1);
	note_SOL(0, 4);

	note_DO(1, 1);
	note_DO(1, 1);
	note_DO(1, 1);
	note_RE(1, 1);
	note_MI(1, 2);
	note_RE(1, 2);

	note_DO(1, 1);
	note_MI(1, 1);
	note_RE(1, 1);
	note_RE(1, 1);
	note_DO(1, 4);
}
