/**
 * @file video.c
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

#include <types.h>
#include <ioports.h>
#include <drivers/video.h>

/* The video memory address. */
#define BASE_VGA_VIDEO                   0xB8000

#define CRT_REG_INDEX 0x3d4
#define CRT_REG_DATA  0x3d5
#define CURSOR_POS_MSB 0x0E
#define CURSOR_POS_LSB 0x0F

#define CTL_COL 0
#define CTL_COL_POS 1
#define CTL_CURSOR 2

#define NB_VGA_PAGES 4

static struct vga_page_t pages[NB_VGA_PAGES];
static int current_page;

void init_video() {
	// Initialisation à 0 du Attribute Mode Control Register pour éviter les surprises...
	inb(0x3DA);
	outb(0x10, 0x3C0);
	outb(0, 0x3C0);
	outb(0x20, 0x3C0);
	inb(0x3DA);

	/* Initialisation des pages vidéos. */
	int i;
	for (i = 0; i < NB_VGA_PAGES; i++) {
		pages[i].front_buffer = (volatile x86_video_mem*) (BASE_VGA_VIDEO + 2
				* i * 0x1000);
		pages[i].back_buffer = (volatile x86_video_mem*) (BASE_VGA_VIDEO + ((2
				* i) + 1) * 0x1000);
	}
	current_page = 0;
}

void offset_video(int offset) {
	/* CRT index port => demande l'accès au registre 0xa ("addr start") */
	outb(0x0C, CRT_REG_INDEX);

	/* scan line start */
	outb(offset >> 8, CRT_REG_DATA);

	/* CRT index port => demande l'accès au registre 0xa ("addr end") */
	outb(0x0D, CRT_REG_INDEX);

	/* scan line end */
	outb(offset & 0x0000FFFF, CRT_REG_DATA);
}

void switch_page(int i) {
	current_page = i;
	paddr_t offset = ((paddr_t) ((paddr_t) pages[i].front_buffer
			- BASE_VGA_VIDEO));

	// J'ai juste passé des heures à essayer de comprendre.
	// Faudrait peut être arriver à changer la taille de la résolution virtuelle pour avoir des pas constants...
	offset_video(offset / 2);
}

void flip_page(int n) {
	volatile x86_video_mem *tmp = pages[n].front_buffer;
	pages[n].front_buffer = pages[n].back_buffer;
	pages[n].back_buffer = tmp;
	if (current_page == n)
		switch_page(current_page);
}

static volatile x86_video_mem* get_page(int i, bool front) {
	if (front) {
		return pages[i].front_buffer;
	} else {
		return pages[i].back_buffer;
	}
}

void set_blink_bit(int blink_bit) {
	int val = 0;

	/* FIXME: Cassé... La lecture renvoie toujours 255...*/
	inb(0x3DA);
	outb(0x10 , 0x3C0);
	val = inb(0x4C1);
	outb(0x20, 0x3C1);
	if (blink_bit) {
		val |= 0x08;
	} else {
		val &= ~0x08;
	}

	if (blink_bit) {
		val = 0x08;
	} else {
		val &= ~0x08;
	}

	/* FIXME: En fait, même ça c'est cassé :D*/
	inb(0x3DA);
	outb(0x10, 0x3C0);
	outb(val, 0x3C0);
	outb(0x20, 0x3C0);
	inb(0x3DA);
}

void disable_cursor(int disable) {
	if (disable) {
		/* CRT index port => demande l'accès au registre 0xa ("cursor start") */
		outb(0x0a, CRT_REG_INDEX);

		/* CRT Register 0xa => bit 5 = cursor OFF */
		outb(1 << 5, CRT_REG_DATA);
	} else {

		/* CRT index port => demande l'accès au registre 0xa ("cursor start") */
		outb(0x0a, CRT_REG_INDEX);

		/* scan line start */
		outb(CURSOR_POS_MSB, CRT_REG_DATA);

		/* CRT index port => demande l'accès au registre 0xa ("cursor end") */
		outb(0x0b, CRT_REG_INDEX);

		/* scan line end */
		outb(CURSOR_POS_LSB, CRT_REG_DATA);
	}
}

void cursor_position_video(int n, int x, int y) {
	int pos = x + y * COLUMNS + n * 4096;

	outb(CURSOR_POS_LSB, CRT_REG_INDEX);
	outb((uint8_t) pos, CRT_REG_DATA);
	outb(CURSOR_POS_MSB, CRT_REG_INDEX);
	outb((uint8_t) (pos >> 8), CRT_REG_DATA);
}

/**
 * Conversion entre latin1 et la table ASCII utilisée.
 */
static char convert_ascii(unsigned char c) {
        if (c == 0xE9) { // é
                c = 0x82;
        } else if (c == 0xE2) { // â
                c = 0x83;
        } else if (c == 0xE4) { // ä
                c = 0x84;
        } else if (c == 0xE0) { // à
                c = 0x85;
        } else if (c == 0xE7) { // ç
                c = 0x87;
        } else if (c == 0xE8) { // è
                c = 0x8a;
        } else if (c == 0xF4) { // ô
                c = 0x93;
        } else if (c == 0xFC) { // ü
                c = 0x81;
        } else if (c == 0xEA) { // ê
                c = 0x88;
        } else if (c == 0xF9) { // ù
                c = 0x97;
        }

        return c;
}

void kputchar_video(int n, bool front, char c, int x, int y, char attr) {
	if (x < COLUMNS && y < LINES) {
		volatile x86_video_mem *video = get_page(n, front);
		c = convert_ascii(c);
		video[x + y * COLUMNS].character = c & 0xFF;
		video[x + y * COLUMNS].attribute = attr;
	}
}

void get_char_video(int n, bool front, char *c, int x, int y, char *attr) {
	if (x < COLUMNS && y < LINES) {
		volatile x86_video_mem *video = get_page(n, front);
		*c = video[x + y * COLUMNS].character;
		*attr = video[x + y * COLUMNS].attribute;
	}
}
